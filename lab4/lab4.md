# Lab 4 
本次 Lab 要在 Raspberry Pi 上撰寫一支名字跑馬燈，大概過程為:
1. 在 Raspberry Pi 上透過 writer 程式將英文字母寫到 driver 當中，
2. 透過 reader 程式將該字從 driver 中讀出來，
3. 最後透過 socket 傳遞給 VM 上頭的 seg.py 程式，其會把該字用十六段顯示器 (GUI) 呈現出來。

## 1. Specification
- driver (自行撰寫)
  - 撰寫自行定義的 write function，將 writer 傳送過來的計數值給存起來。
  - 撰寫自行定義的 read function, 將經過處理之後的計數值回傳給使用者。
  - 所謂的處理，就是將字轉換為16段顯示器可以接受的資料格式 - 1 個長度為 16 的陣列，每一格儲存的不是 0 就是 1。
  - 16段顯示器顯示資訊表:
```
bits_for_seg = (
​​​​ 1st  bit  # top left
​​​​ 2nd  bit  # top right
​​​​ 3rd  bit  # upper right
​​​​ 4th  bit  # lower right
​​​​ 5th  bit  # bottom right
​​​​ 6th  bit  # bottom left
​​​​ 7th  bit  # lower left
​​​​ 8th  bit  # upper left
​​​​ 9th  bit  # upper left slash
​​​​10th  bit  # upper middle
​​​​11th  bit  # upper right slash
​​​​12th  bit  # middle right
​​​​13th  bit  # lower right slash
​​​​14th  bit  # upper middle
​​​​15th  bit  # lower left slash
​​​​16th  bit  # middle left
​​​​)
​​​​
​​​​seg_for_c[27] = {
​​​​    0b1111001100010001, // A
​​​​    0b0000011100000101, // b
​​​​    0b1100111100000000, // C
​​​​    0b0000011001000101, // d
​​​​    0b1000011100000001, // E
​​​​    0b1000001100000001, // F
​​​​    0b1001111100010000, // G
​​​​    0b0011001100010001, // H
​​​​    0b1100110001000100, // I
​​​​    0b1100010001000100, // J
​​​​    0b0000000001101100, // K
​​​​    0b0000111100000000, // L
​​​​    0b0011001110100000, // M
​​​​    0b0011001110001000, // N
​​​​    0b1111111100000000, // O
​​​​    0b1000001101000001, // P
​​​​    0b0111000001010000, //q
​​​​    0b1110001100011001, //R
​​​​    0b1101110100010001, //S
​​​​    0b1100000001000100, //T
​​​​    0b0011111100000000, //U
​​​​    0b0000001100100010, //V
​​​​    0b0011001100001010, //W
​​​​    0b0000000010101010, //X
​​​​    0b0000000010100100, //Y
​​​​    0b1100110000100010, //Z
​​​​    0b0000000000000000
​​​​};
```
  - ![image](https://hackmd.io/_uploads/rJSkq8qJJl.png)
