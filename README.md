# np_hw2
聊天室server

請實作一個multi-thread版的群組聊天室server，也就是server會建立一個新的thread來服務每個client。

client在剛連上server時會送出自己的帳號名稱(這裡不做密碼認證)，之後client可以 

(1) 查看有哪些使用者正在server線上 

(2) 送出訊息給所有連到同一server的client 

(2) 指定傳送訊息給正在線上的某個使用者 

(3) 也可以傳送檔案給另一個使用者，對方可決定是否接收 

作業demo時須至少展示有三個使用者上線。

referrence: https://www.itread01.com/content/1533623883.html?fbclid=IwAR0X4i_h7tKGlJMRppEWCGj3ttgX7KatoKG-X41jk1FCgYgU0Pt7RxTYn08