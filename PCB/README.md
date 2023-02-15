大创项目的shield设计文件  
包含：lcd_shield，board_shield  
使用立创EDA设计  

*立创eda打开方式  
解压嘉立创工程文件，里面有原理图与PCB的.json文件  
打开编辑器，通过：顶部菜单 - 文件 - 打开 - 立创EDA... ，选择 json 文件打开在编辑器，你可以保存文档进工程里面。

*进度   
2023/2/10   完成设计与焊接  

*更新日志  
2023/2/10  
上传文件  嘉立创工程文件：bjut_v2_20230210,bjut_v2_20230210  制板文件：Gerber_bjuts_v2,Gerber_PCB_lcd_tr  
bjut_v2：board_shield ，实现uno、esp、yf-s201、推进器等模块连接，注意5V供电  
lcd_tr：lcd_shield，实现LCD-1602连接，与board_shield的lcd排母对应，lcd的VL与可变电阻相连  

*BOM清单  
bjuts_v2：

|      | Name                  | Designator | Footprint              | Quantity |
| ---- | --------------------- | ---------- | ---------------------- | -------- |
| 1    | DC005-2.0MM           | DC1        | DC-IN-TH_DC005         | 1        |
| 2    | 180Ω                  | R3         | R1206                  | 1        |
| 3    | FC-2012UGK-520D5      | LED1       | LED0805-R-RD           | 1        |
| 4    | PM254-1-16-Z-8.5      | LCD        | HDR-TH_16P-P2.54-V-F-A | 1        |
| 5    | Header-Male-2.54_1x3  | YF-S201,P  | HDR-TH_3P-P2.54-V-M-1  | 2        |
| 6    | DSWB01LHGET           | SW1        | SW-TH_DSWB01LHGET      | 1        |
| 7    | 2.54-1*4P母           | USB-TTL    | HDR-TH_4P-P2.54-V-F    | 1        |
| 8    | Header-Male-2.54_1x10 | H3         | HDR-TH_10P-P2.54-V-M-1 | 1        |
| 9    | Header-Male-2.54_1x8  | H4,H5      | HDR-TH_8P-P2.54-V-M    | 2        |
| 10   | Header-Male-2.54_1x6  | H6         | HDR-TH_6P-P2.54-V-M-1  | 1        |
| 11   | PM254-1-15-Z-8.5      | U2,U3      | HDR-TH_15P-P2.54-V-F   | 2        |

lcd_tr：

| ID   | Name               | Designator | Footprint                             | Quantity |
| ---- | ------------------ | ---------- | ------------------------------------- | -------- |
| 1    | X6511WV-16H-C30D60 | H1         | HDR-TH_16P-P2.54-V-M                  | 1        |
| 2    | PM254-1-16-Z-8.5   | LCD1       | HDR-TH_16P-P2.54-V-F-A                | 1        |
| 3    | 50Ω                | R1         | RES-ADJ-TH_3P-L9.5-W4.9-P2.50-L_3296W | 1        |
