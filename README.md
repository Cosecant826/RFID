# RFID
#24练习题1  
##一、任务  
设计并制作超市刷卡系统。开机后，屏幕第一行显示“CSSKXT”，第二行显示“抽签号后2位数”（如07），并自左而右滚动，3秒后停止滚动。画出系统各组件连接图，并简要说明，画出全部程序流程图。编写控制器使用说明书。必须使用答题纸作答，竞赛结束上交。

##二、功能要求  
（1）能够通过IC卡进行预存现金，每次存入金额大小，A卡存入100元，B卡存入200元。  
（2）通过称重传感器测量砝码克数，利用矩阵键盘输入水果编码（10021苹果5.6元/公斤，10022香蕉7.2元/公斤，10023桔子10.3元/公斤），通过12864LCD点阵屏显示屏实时显示（显示内容：品名，单价，重量，价格XX.XX元）。其中：砝码每克对应系统的50克。  
（3）能够实现刷卡消费操作，将购买水果金额从卡中扣除，并能通过液晶显示消费金额和卡中余额。刷卡完毕后，可以语音播报“已付款+消费金额“。  
（4）能够完成多种水果的连续测量和计价并显示，并可以删除其中不想要的水果，然后重新计价。  
（5）能够通过按键输入新的水果编码，价格操作（新水果10024 香瓜，11.7元/公斤），并开机不丢失。  
（6）能够记录当天消费最大金额及消费时间，能够记录三种水果的累计销量（以公斤作为单位）  


##按键功能  
按键1  
{  
   主界面&新增商品：输入数字  
   购物车：光标上移  
}  
按键2  
{  
   主界面&新增商品：输入数字  
   购物车：光标下移  
}  
按键3-10输入数字  
按键11退格  
按键12  
{  
  计价界面：确认价格，回到主页  
  购物车：移除当前商品  
}  
按键13  
{  
  内层界面：返回上一层界面  
  主界面：进入充值界面-蓝卡每次100元；白卡每次200元（任意键退出）  
}  
按键14显示本日最大金额消费及时间（任意键退出）  
按键15显示销量统计（任意键退出）  
按键16  
{  
   主界面：新增商品  
   购物车：确认付款（任意键退出）  
}  
