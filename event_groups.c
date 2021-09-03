#include "freertos/event_groups.h"
 
/*定义事件位 */
#define TASK_1_BIT        ( 1 << 0 ) //1
#define TASK_2_BIT        ( 1 << 1 ) //10
#define TASK_3_BIT        ( 1 << 2 ) //100
#define ALL_SYNC_BITS (TASK_1_BIT | TASK_2_BIT | TASK_3_BIT) //111
 
 
/* 创建硬件定时器 */
hw_timer_t * timer = NULL;
/* 创建事件组 */
EventGroupHandle_t eg;
int count = 0;
 
/* timer ISR callback */
void IRAM_ATTR onTimer(){
  BaseType_t xHigherPriorityTaskWoken;
  count++;
  if(count == 2){
    /* 如果counter等于2则设置 task1 */
    xEventGroupSetBitsFromISR(eg,TASK_1_BIT, &xHigherPriorityTaskWoken);
  }else if(count == 3){
    /* 如果counter等于3然后设置任务2的事件位和3 * */
    xEventGroupSetBitsFromISR(eg,TASK_2_BIT | TASK_3_BIT, &xHigherPriorityTaskWoken);
  }else if(count == 4){
    /*  counter再次启动 */
    count = 0;
  }
}
 
void setup() {
 
  Serial.begin(112500);
  eg = xEventGroupCreate();
 
 
  /* 1/(80MHZ/80) = 1us  */
  timer = timerBegin(0, 80, true);
 
  /* 将onTimer函数附加到我们的计时器 */
  timerAttachInterrupt(timer, &onTimer, true);
 
  /* 设置闹钟每秒调用onTimer函数1 tick为1us 
  => 1秒为1000000us*/
  /* 重复闹钟（第三个参数） */
  timerAlarmWrite(timer, 1000000, true);
 
  /* 发出警报 */
  timerAlarmEnable(timer);
  Serial.println("start timer");
  
  xTaskCreate(
      task1,           /* 任务功能 */
      "task1",        /* 任务名称. */
      10000,                    /* 任务的堆栈大小 */
      NULL,                     /* 任务的参数*/
      1,                        /* 任务的优先级 */
      NULL);                    /* 跟踪创建任务的任务句柄 */
  xTaskCreate(
      task2,           
      "task2",        
      10000,                    
      NULL,                   
      1,                       
      NULL);                    
  xTaskCreate(
      task3,          
      "task3",        
      10000,                   
      NULL,                     
      1,                        
      NULL);                    
}
 
void loop() {
 
}
 
void task1( void * parameter )
{
  for(;;){
    /* 永远等待，直到任务1的事件位被设置为 */
    EventBits_t xbit = xEventGroupWaitBits(eg, TASK_1_BIT, pdTRUE, pdTRUE, portMAX_DELAY);
    Serial.print("task1 has even bit: ");
    Serial.println(xbit);
  }
  vTaskDelete( NULL );
}
/* 此任务类似于sendTask1 */
void task2( void * parameter )
{
 
  for(;;){
    
    EventBits_t xbit = xEventGroupWaitBits(eg, TASK_2_BIT, pdTRUE, pdTRUE, portMAX_DELAY);
    Serial.print("task2 has even bit: ");
    Serial.println(xbit);
  }
  vTaskDelete( NULL );
}
void task3( void * parameter )
{
  for(;;){
   
    EventBits_t xbit = xEventGroupWaitBits(eg, TASK_3_BIT, pdTRUE, pdTRUE, portMAX_DELAY);
    Serial.print("task3 has even bit: ");
    Serial.println(xbit);
  }
  vTaskDelete( NULL );
}