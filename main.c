/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ili9488.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define PADDING_TOP	  60
#define PADDING_LEFT  0
#define BOX_LEFT      10
#define BOX_RIGHT     (ILI9488_WIDTH - 10)
#define BOX_TOP       70
#define BOX_BOTTOM    (ILI9488_HEIGHT - 10)
#define GRID_X_MIN    (BOX_LEFT / GRID_SIZE)
#define GRID_X_MAX    (BOX_RIGHT / GRID_SIZE)
#define GRID_Y_MIN    (BOX_TOP / GRID_SIZE)
#define GRID_Y_MAX    (BOX_BOTTOM / GRID_SIZE)
#define GRID_SIZE 10
#define BOX_WIDTH 300
#define BOX_HEIGHT 400
#define MAX_SNAKE_LEN ((BOX_WIDTH / GRID_SIZE) * (BOX_HEIGHT / GRID_SIZE))
#define SNAKE_INIT_X (BOX_LEFT + 30 + GRID_SIZE) / GRID_SIZE
#define SNAKE_INIT_Y (BOX_TOP + BOX_HEIGHT / 2) / GRID_SIZE
#define INVALID_COORD (uint16_t)(-1)

#define COLOR_BG     ILI9488_BLACK
#define COLOR_TEXT   ILI9488_WHITE
#define COLOR_SELECT ILI9488_YELLOW
#define DELAY_MIN 50
#define DELAY_NOR 100
#define DELAY_MAX 150
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
typedef enum {
    MENU_PLAY,
    MENU_SETTING,
    MENU_HIGHSCORE,
	MENU_COUNT
} menu_option_t;
typedef enum { NONE = -1, UP, DOWN, LEFT, RIGHT} snake_dir;
typedef enum { PLAYING, CRASHED, WON } snake_state;
typedef enum { WAITING, PLACED, EATEN } food_state;

typedef struct {
    int x;
    int y;
} coord_t;

typedef struct {
	coord_t coord;
	food_state state;
} food_t;

typedef struct {
	snake_dir direction;
	coord_t body[MAX_SNAKE_LEN];
	uint16_t length;
	coord_t ghost;
	snake_state state;
} snake_t;

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN PV */
snake_t snake;
food_t food;
uint8_t selected = 0;
uint16_t gameDelay = 100;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void drawMenu(uint8_t selected) {
    const char* options[MENU_COUNT] = { "Play", "Setting", "Highscore" };

    for (uint8_t i = 0; i < MENU_COUNT; i++) {
        uint16_t color = (i == selected) ? COLOR_SELECT : COLOR_TEXT;
        printText((char*)options[i], 100, 80 + i * 40, color, COLOR_BG);
        HAL_Delay(100);
    }
}

uint16_t spawnFood(snake_t* snake, food_t* food) {
	uint16_t isInvalid = 0;

	do
	{
        food->coord.x = GRID_X_MIN + rand() % (GRID_X_MAX - GRID_X_MIN);
        food->coord.y = GRID_Y_MIN + rand() % (GRID_Y_MAX - GRID_Y_MIN);

        for (int i = 0; i < snake->length; i++) {
            if (snake->body[i].x == food->coord.x && snake->body[i].y == food->coord.y) {
                isInvalid = 1;
                break;
            }
        }
	} while (isInvalid);

	if (!isInvalid)
	{
    	fillRect(food->coord.x * GRID_SIZE, food->coord.y * GRID_SIZE, GRID_SIZE, GRID_SIZE, ILI9488_RED);
	}

	return isInvalid;
}

void generateFood(snake_t* snake, food_t* food) {
    if (food->state != PLACED)
    {
        if (spawnFood(snake, food) == 1)
        {
            fillScreen(ILI9488_BLACK);
        }
        else
        {
            food->state = PLACED;
        }
    }
}

void drawBorder() {
	drawBox(PADDING_LEFT, PADDING_TOP, 320, 420, ILI9488_YELLOW);
}

void snakeInit(snake_t* snake) {
	snake->length = 3;
	snake->direction = RIGHT;
	snake->ghost.x = INVALID_COORD;
	snake->ghost.y = INVALID_COORD;
	memset(&snake->body[0], 0, MAX_SNAKE_LEN*sizeof(coord_t));
	for (int i = 0; i < snake->length; i++) {
		snake->body[i].x = SNAKE_INIT_X + i;
		snake->body[i].y = SNAKE_INIT_Y;
	}
}

void drawSnake(snake_t* snake) {
	if (snake->ghost.x != INVALID_COORD && snake->ghost.y != INVALID_COORD)
	{
        fillRect((snake->ghost.x - GRID_X_MIN) * GRID_SIZE + BOX_LEFT,
                 (snake->ghost.y - GRID_Y_MIN) * GRID_SIZE + BOX_TOP,
                 GRID_SIZE, GRID_SIZE, ILI9488_BLACK);
	}
    for (int i = 0; i < snake->length; i++) {
        int x = (snake->body[i].x - GRID_X_MIN) * GRID_SIZE + BOX_LEFT;
        int y = (snake->body[i].y - GRID_Y_MIN) * GRID_SIZE + BOX_TOP;
        uint16_t color = (i == snake->length - 1) ? ILI9488_YELLOW : ILI9488_RED;
        fillRect(x, y, GRID_SIZE, GRID_SIZE, color);
    }
}

void moveSnake(snake_t* snake) {
	snake->ghost = snake->body[0];

	// Move body forward
	memmove(&snake->body[0], &snake->body[1],sizeof(coord_t) * (snake->length - 1));

	coord_t* head = &snake->body[snake->length - 1];
	int next_x = head->x;
	int next_y = head->y;

	switch (snake->direction)
	{
	case UP:	next_y--; break;
	case DOWN: 	next_y++; break;
	case RIGHT:	next_x++; break;
	case LEFT: 	next_x--; break;
	default: {};
	}

	// Check wall collision
	if (next_x < GRID_X_MIN || next_x >= GRID_X_MAX || next_y < GRID_Y_MIN || next_y >= GRID_Y_MAX)
	{
		snake->state = CRASHED;
		return;
	}

	// Check self collision
	for (int i = 0; i < snake->length; i++) {
		if (snake->body[i].x == next_x && snake->body[i].y == next_y)
		{
			snake->state = CRASHED;
			return;
		}
	}

	head->x = next_x;
	head->y = next_y;

	if (snake->length == MAX_SNAKE_LEN)
	{
		snake->state = WON;
	}
}

void snakeEat(snake_t* snake, food_t* food) {
	if ((snake->body[snake->length - 1].x == food->coord.x) && (snake->body[snake->length - 1].y == food->coord.y))
	{
        memmove(&snake->body[1], &snake->body[0], sizeof(coord_t) * snake->length);
		snake->body[0] = snake->ghost;
		snake->ghost.x = INVALID_COORD;
		snake->ghost.y = INVALID_COORD;
		snake->length++;

		food->state = EATEN;
	}
}

void getDirection(snake_t* snake)
{
	static snake_dir direction = NONE;
    if (!HAL_GPIO_ReadPin(GPIOB, BUTTON_DOWN_Pin)) {
    	direction = DOWN;
    }
    else if (!HAL_GPIO_ReadPin(GPIOA, BUTTON_UP_Pin)) {
    	direction = UP;
    }
    else if (!HAL_GPIO_ReadPin(GPIOB, BUTTON_RIGHT_Pin)) {
    	direction = RIGHT;
    }
    else if (!HAL_GPIO_ReadPin(GPIOB, BUTTON_LEFT_Pin)) {
    	direction = LEFT;
    }

    // Only update if not opposite direction
	if (!(snake->direction == LEFT && direction == RIGHT) &&
		!(snake->direction == RIGHT && direction == LEFT) &&
		!(snake->direction == UP && direction == DOWN) &&
		!(snake->direction == DOWN && direction == UP))
    {
        snake->direction = direction;
    }
}

void startSnakeGame()
{
    clearScreen(ILI9488_BLACK);
    drawBorder();

    snake_t snake = {0};
    food_t food = {0};

    snakeInit(&snake);
    while (1)
    {
        getDirection(&snake);
        moveSnake(&snake);
        snakeEat(&snake, &food);
        drawSnake(&snake);
        generateFood(&snake, &food);
          // Exit condition for game ???????????


        HAL_Delay(gameDelay);
    }
}

void settingsMenu() {
    const char* options[] = { "Fast", "Normal", "Slow"};
    const uint8_t optionCount = sizeof(options) / sizeof(options[0]);
    uint8_t selected = 1;
    fillScreen(COLOR_BG);

    while (1) {
        printText("Setting", 100, 40, COLOR_SELECT, COLOR_BG);

        for (uint8_t i = 0; i < optionCount; i++) {
            uint16_t color = (i == selected) ? COLOR_SELECT : COLOR_TEXT;
            printText((char*)options[i], 80, 100 + i * 40, color, COLOR_BG);
        }

        if (!HAL_GPIO_ReadPin(GPIOA, BUTTON_UP_Pin)) {
            if (selected > 0) selected--;
            HAL_Delay(100);
        }
        else if (!HAL_GPIO_ReadPin(GPIOB, BUTTON_DOWN_Pin)) {
            if (selected < optionCount - 1) selected++;
            HAL_Delay(100);
        }
        else if (!HAL_GPIO_ReadPin(GPIOB, BUTTON_RIGHT_Pin)) {
            switch (selected) {
                case 0: gameDelay = DELAY_MIN; break;   // Fast
                case 1: gameDelay = DELAY_NOR; break; // Normal
                case 2: gameDelay = DELAY_MAX; break;   // Slow
            }
        }
        else if (!HAL_GPIO_ReadPin(GPIOB, BUTTON_LEFT_Pin)) {
            fillScreen(ILI9488_BLACK);
            return;
        }
    }
}

void mainMenu() {
    drawMenu(selected);
    printText("Hello WORLD", 20, 30, ILI9488_WHITE, ILI9488_BLACK);

    while (1) {
        if (!HAL_GPIO_ReadPin(GPIOA, BUTTON_UP_Pin)) {
            if (selected > 0) selected--;
            drawMenu(selected);
        }
        else if (!HAL_GPIO_ReadPin(GPIOB, BUTTON_DOWN_Pin)) {
            if (selected < MENU_COUNT - 1) selected++;
            drawMenu(selected);
        }
        else if (!HAL_GPIO_ReadPin(GPIOB, BUTTON_RIGHT_Pin)) {
            switch (selected) {
                case MENU_PLAY:
                    startSnakeGame();
                    drawMenu(selected);
                    break;
                case MENU_SETTING:
                    settingsMenu();
                    drawMenu(selected);
                    break;
                case MENU_HIGHSCORE:
// add highscore
                    drawMenu(selected);
                    break;
            }
        }
    }
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
  ILI9488_Init();
  setRotation(2);
  fillScreen(ILI9488_BLACK);
  mainMenu();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : BUTTON_DOWN_Pin BUTTON_LEFT_Pin BUTTON_RIGHT_Pin */
  GPIO_InitStruct.Pin = BUTTON_DOWN_Pin|BUTTON_LEFT_Pin|BUTTON_RIGHT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : LCD_RESET_Pin */
  GPIO_InitStruct.Pin = LCD_RESET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LCD_RESET_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BUTTON_UP_Pin */
  GPIO_InitStruct.Pin = BUTTON_UP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(BUTTON_UP_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LCD_CS_Pin */
  GPIO_InitStruct.Pin = LCD_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LCD_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LCD_DC_Pin */
  GPIO_InitStruct.Pin = LCD_DC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LCD_DC_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
