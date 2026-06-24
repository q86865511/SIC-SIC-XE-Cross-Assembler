# SIC / SIC-XE Two-Pass Cross-Assembler

> 用 C++ 從零實作的 SIC 與 SIC/XE 兩遍式（two-pass）交叉組譯器，將組合語言原始碼翻譯成位置（Loc）與目的碼（Object Code）。中原大學「系統程式（System Programming）」課堂專案 — 作業 2，成績 91/100。

![language](https://img.shields.io/badge/language-C%2B%2B-00599C)
![platform](https://img.shields.io/badge/platform-Windows-blue)
![license](https://img.shields.io/badge/License-MIT-green)

---

## 問題（這個程式做什麼）

組譯器（Assembler）是系統程式課程的核心題目：把人類可讀的組合語言，逐行翻譯成機器能執行的目的碼。本專案針對教科書中的 **SIC** 與其延伸架構 **SIC/XE** 兩種指令集，實作一支能夠：

1. 讀取 SIC 或 SIC/XE 的組合語言原始碼，
2. 以 **兩遍掃描（Two-Pass）** 計算每一行指令的位置計數器（Location Counter），並建立符號表（Symbol Table），
3. 最終輸出每一行的 **Line / Loc / 原始敘述 / Object Code** 對照表。

同一支程式可在 SIC 與 SIC/XE 兩種模式間切換，因此稱為「Cross-Assembler（交叉組譯器）」。

## 方法與技術

這是一個**底層／系統程式**題材，重點在於手刻組譯流程中的各個資料結構與演算法，而非套用現成函式庫。整支程式為單一 C++ 檔（`Assembler_10827129.cpp`，約 2,300 行），由三個 class 組成：

- **`Lexer`（語彙分析器 / Tokenizer）**
  逐字元掃描原始碼，依分隔符（delimiter）與空白切出 token；以雜湊（`ascii % 100`、線性探測）把符號、整數、字串分別存進對應的雜湊表（第 5/6/7 號表），並記錄每一行的 token 數，供後續以「一行一行、每行數個 token」的格式重建。

- **`Table`（指令／符號對照表）**
  從外部檔案 `Table1.table`～`Table4.table` 載入四張靜態表：
  - Table1：指令助記符（opcode mnemonics，如 `lda`、`jsub`、`comp`…）
  - Table2：虛擬指令 / pseudo-op（`START`、`END`、`BYTE`、`WORD`、`RESB`、`RESW`、`EQU`、`BASE`、`LTORG`）
  - Table3：暫存器（`A`、`X`、`L`、`B`、`S`、`T`、`F`、`PC`、`SW`）
  - Table4：分隔符號（`, + - * / : ; ? ' . = # @`）

- **`Assembler`（核心：兩遍式翻譯機）**
  - `setInstructureSet()`：建立 59 筆指令集，每筆記錄 **opcode、運算元個數、指令格式（Format 1/2/3/4）、是否限 SIC/XE 專用**。
  - `pass1()`：第一遍計算 Location Counter 與 PC、把標籤（label）位址寫入符號表、偵測**重複定義（multiple definition）**錯誤、處理位址相關的 pseudo-op（`RESB`/`RESW`/`BYTE`/`WORD`/`EQU`/`START`…）。
  - `pass2()`：第二遍逐 token 判斷型別（指令 / pseudo / 暫存器 / 分隔符 / 符號 / 整數 / 字串），偵測**未定義符號**，並產生目的碼。
  - SIC/XE 額外支援：**Format 1～4**、定址旗標 **n / i / x / b / p / e**、立即定址 `#`、間接定址 `@`、Format 4 的 `+` 前綴、`BASE` 暫存器與 base-relative 計算、`LTORG` 與 **Literal Table（字面常數表）** 的位址回填。
  - 自行手刻數值轉換：十進位↔十六進位、整數↔二進位字串、再把組好的位元欄位轉回十六進位目的碼。

> 技術亮點：符號表（雜湊 + 線性探測）、字面常數表、兩遍式位置計算、指令格式與定址模式的旗標位元組裝、目的碼產生，皆為手動實作，完整呈現組譯器內部運作。

## 結果

對教科書經典的 SIC `COPY` 程式（`SIC_input.txt`）執行後，產生位置與目的碼對照表（節錄自 `SIC_input_output.txt`）：

```
Line    Loc     Source statement                Object code
5       1000    COPY    START   1000
10      1000    FIRST   STL     RETADR          001009
15      1000    CLOOP   JSUB    RDREC           00200F
20      1000            LDA     LENGTH          00100C
...
80      1000    EOF     BYTE    C'EOF'          454F46
85      1003    THREE   WORD    3               000003
...
250     2013    OUTPUT  BYTE    X'05'           05
255             END     FIRST
```

- 正確處理 `STL` / `JSUB` / `LDA` 等指令的 opcode 與位址。
- `BYTE C'EOF'` 轉為 ASCII 目的碼 `454F46`、`BYTE X'05'` 轉為 `05`、`WORD 3` 轉為 `000003`。
- `RESW` / `RESB` 正確推進 Location Counter 而不產生目的碼。

倉庫內的範例檔：

| 檔案 | 說明 |
| --- | --- |
| `SIC_input.txt` | SIC 範例輸入（`COPY` 程式） |
| `SIC_input_output.txt` | 上述輸入的 SIC 輸出結果 |
| `SICXE_input.txt` | SIC/XE 範例輸入 |
| `Table1.table`～`Table4.table` | 指令 / pseudo-op / 暫存器 / 分隔符對照表（執行時必讀） |

## 如何 build 與執行

本程式為**單檔 C++**，沒有 makefile 或 IDE 專案檔（編譯產物已自版控移除）。原始開發環境為 **Windows + Dev-C++**。程式用到 Windows / MSVC 特有的 `<io.h>`、`itoa()` 與 `system("pause")`，因此建議在 **Windows** 上以支援這些 API 的工具鏈編譯（Dev-C++ 內建的 MinGW，或一般 g++）。

```bash
# 進入原始碼目錄（重要：table 檔與輸入檔都用相對路徑讀取，須在此目錄執行）
cd Assembler_10827129

# 以 g++（MinGW）編譯
g++ Assembler_10827129.cpp -o assembler.exe

# 執行
./assembler.exe
```

或直接用 **Dev-C++** 開啟 `Assembler_10827129.cpp`，按 Compile & Run。

執行後依互動提示操作：

1. **輸入檔名**：需含副檔名，例如 `SIC_input.txt`（輸入 `0` 可結束程式）。
2. **選擇模式**：`0` → SIC、`1` → SICXE。
3. 程式自動產生輸出檔，命名規則：
   - SIC：`SIC_input.txt` → `SIC_input_output.txt`
   - SIC/XE：`SICXE_input.txt` → `SICXE_input_outputXE.txt`
4. 完成後輸入 `1` 可繼續組譯下一個檔案，輸入 `0` 結束。

> 注意：`Table1.table`～`Table4.table` 以及輸入檔都以**相對路徑**讀取，務必在 `Assembler_10827129/` 目錄下執行，否則會找不到表檔或輸入檔。

## 已知限制

以下為作者在原始碼與說明中標註、或經程式碼驗證的限制：

- **平台相依**：使用 `<io.h>`、`itoa()`、`system("pause")` 等 Windows / MSVC 專屬 API，非跨平台，於 Linux / macOS 需修改後才能編譯。
- **互動式 CLI**：只能透過主控台逐步輸入檔名與模式，無命令列參數或批次模式。
- **輸出格式空白未完全對齊**：欄位（white space）與教科書範例不完全一致。
- **Literal 與 EQU 尚未完善**：多個 literal 與 EQU 並存時，目的碼偶爾出現少 3 的情況。
- **EQU 算式支援有限**：僅支援兩個 symbol 的單一加減（`A+B`、`A-B`）；乘除、多項運算、括號皆未處理。
- **Line 編號計法**：採 `+5` 遞增，與課本部分例題的算法不同（作者保留現狀未改）。
- **SIC/XE 範例輸出未完整**：倉庫內的 `SICXE_i_outputXE.txt` 僅含表頭，未含完整目的碼；SIC/XE 路徑的完整輸出請自行以上述步驟重新產生並驗證。

---

*課程：中原大學 系統程式（System Programming）— 作業 2 ｜ 成績 91 / 100*
