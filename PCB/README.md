流速计的shield设计文件  
包含：lcd_shield，board_shield  
使用立创EDA设计  

*立创eda打开方式
解压嘉立创工程文件，可以看到原理图与PCB的.json文件  
打开编辑器，通过：顶部菜单 - 文件 - 打开 - 立创EDA，选择。json文件打开在编辑器，你可以保存文档进工程里面。

*进度   
2023/2/10  完成设计与焊接  

*更新日志  
2023/2/10  
上传文件  嘉立创工程文件：bjut_v2_20230210,lcd_tr_20230210  制板文件：Gerber_bjuts_v2,Gerber_PCB_lcd_tr
bjut_v2：基础shield，实现esp32、uno、lcd、yfs201、推进器等模块连接  
lcd_tr：lcd_shield，用于连接LCD1602，其中lcd的VL与可调电阻连接
