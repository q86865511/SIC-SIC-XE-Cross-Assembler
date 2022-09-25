# SIC-SIC-XE-Cross-Assembler
開發一款能夠正確讀取SIC &amp; SIC/XE的程式碼、記錄其位置，透過Two pass分析指令，最後輸出指令的位置與Object code的程式。

# 結果圖
![image](https://user-images.githubusercontent.com/95120819/192145966-60eb6aad-305c-4423-a1e4-7c3c5f7ef5ae.png)

# 開發平台
Windows 10, Intel(R) Core(TM) i5-3470 CPU @ 3.20GHz   3.60 GHz 
Dev C++

# 程式使用方式
一、輸入檔案名稱 : 須包含.txt等等的副檔名ex:SIC_input.txt

二、輸入Assembler型式 : 0 -> SIC, 1 -> SICXE

三、輸出檔案名稱 : 

SIC : SIC_input.txt -> SIC_input_output.txt

SIC/XE : SICXE_input.txt -> SICXE_input_outputXE.txt

四、若要繼續使用輸入1，否則0，1 -> 回到(一)

# 程式架構與流程說明：
三個class,Lexer(Lexical analyzer) Table Assembler, Lexer負責處理切token，也是存切完的結果的地方(以一行一行存數個token的方式)，

而Table作為存7個table的資料結構，最後是Assembler，主要就是這次作業的重點，翻譯機器碼。

其中包含 :

紀錄當前位置的loc_Counter_Temp

紀錄起始位置的start_Location

紀錄下個指令的位置的pc

紀錄EQU的位置的locTempForEQU

儲存各種狀態的布林變數(isByte、isEQU、isXE等等)

七個Tables和Literal Table

儲存最後結果的Assembler List。

# 程式執行的流程
程式會決定要將輸入檔案翻成SIC或是SICXE的機器碼，為了便於將指令翻成機器碼，因此需要前面程式作業一的切完Token

後的結果，所以在初始化Assembler ab時，順便將所有的Tables送入其內，接下來呼叫setInstructureSet()設定指令集的內容(包括該指令的Opcode、運算元個數、格式、是否限用於SICXE)，

到這裡所以的前置動作都已經完成，嚴格來說前面再處理切Token的動作已經花費一個Pass，扣除掉切Token的那個Pass，以下說明會以原有的Pass 1和Pass 2來稱呼。再來，呼叫ab.pass1()，

Pass 1會記錄每一指令對應到的Line和Loc，若遇到Symbol會記錄該Symbol的地址於Symbol Table中，在此階段若有發現多重定義則會輸出錯誤訊息，

同時也會根據Instruction Type和指令對照的Opcode來設定對應的Opcode欄位，並依序將每一行的結果存入Assembler List中，

設定完之後會初始化指令所造成的狀態以供下一道指令做處理，處理完Pass 1之後會對程式的Line和Loc初始化，然後處理基本的Pseudo code 問題(ex:pc,address等等)，

緊接著，呼叫ab.pass2()，和Pass 1 同樣也會計算Loc和Line，在此階段會檢測到是否有還沒有定義的Symbol，若有則會輸出錯誤訊息告知使用者，而在翻機器碼的過程，

首先會判斷遇到的指令可能是Pseudo或是Excutable，如果是Pseudo則不翻成機器碼，反之則是Excutable要翻出機器碼，以下為各種Pseudo的處理方法：

START：設定start_Location。

BYTE：以BYTE進行定義，機器碼欄位出現是對應的數值或是ASCII碼。

WORD：以WORD進行定義，機器碼欄位出現的是所定義的數值。

RESB：保留多少BYTE。

RESW：保留多少WORD。

END：遇到END會將地址賦予Literal Table中的Literal。

EQU：EQU的前方是要被定義的Label，後方則為其地址。

BASE：設定BASERegister中的值，只有SICXE有這個功能

LTORG：LTORG會將地址賦予Literal Table中的Literal，只有SICXE有。


關於Excutable指令的處理方式，首先必須要判斷處理的Token的類型，可以分為7種不同的Type，會依據不同的Type做不同的處理，如下歸類：

Type 1：屬於可執行的指令，若是SICXE則會根據指令，決定格式以及機器碼中的部分內容。

Type 2：是Pseudo，作法如上面所提。

Type 3：是Register，若當前指令是Format 2則設定r1和r2的欄位，若為
Format 3則是使用到Index Register。

Type 4：是Delimiter，考慮到有注解的情況。

Type 5：是Symbol，如果是isBASE則將BASERegister的值設為該Symbol的地址，若前一個Token是"#"則判定為立即定址，還有"@"，若非以上情況且為Format 3就必須計算Disp，
而Format 4只需設定Address欄位即可。

Type 6：是Integer，若為Format 2則要設定r1或r2，並判斷是Format 3或4以及有沒有”#”做出相對的處理。

Type 7：是String，如果是Literal的情況才會對機器碼的產生造成影響，若為Format 3要做Disp的計算，Format 4則要設定Address，其他情況則屬於String的定義。

最後會將處理後的機器碼分別設定到Assembler List中，並且呼叫ab.writeFile()於路徑下輸出正確的檔案。

# 未完成的功能
格式上並未完全相同(white space)，且literal和EQU並沒有到完美，自己測的時候，若有多道literal和EQU存在，有時會出現object code少3的情況，還有就是若EQU後方是算式，

且不為單純兩個symbol加減，例如乘法除法或是多個數字做運算等等的問題，都還未處理，但如果是(A+B ,A-B)則可以處理，還有括號無處理，最後是課本上的line運算有些不同，不為+5+5+5如此，

我不太清楚是否要更改，因此並未如課本上的例題一樣(目前仍是+5+5+5)。

# 備註
中原大學SP系統程式作業2
