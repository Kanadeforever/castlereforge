; 《幽城幻剑录》资源容器头部、索引与四模式Loader静态证据
; 固化版本：v0.7D阶段中-固化24
; 输入：RPG.exe
; SHA256：b10c65f56051e5a625b6c34857bcb73bd002efe3c158b6bd0cc2bb17fa871dcf
; 反汇编：GNU objdump -d -Mintel
; 规则：每个片段都从已确认指令边界开始；跳表按原始DWORD列出，不把数据误解码成指令。
;
; 结论边界：
; - Map/SYS/Music初始化读取10-byte头，只消费头内+6索引字节数；
; - 以/39-1得到跳过根记录后的缓存项数，缓存从绝对偏移49开始；
; - Loader 0x4025B0有mode 0/1/2/3四项；
; - 容器查找按record+10名称、stride 39，命中返回+2大小和+35绝对数据偏移；
; - 头+0/+4和记录+6未在这些直接路径中获得语义，继续UNKNOWN。

; -----------------------------------------------------------------------------
; 当前Map DAT初始化：0x00402190
; -----------------------------------------------------------------------------
00402190  A0 FA F5 46 00           mov  al, byte ptr [0046F5FA]
00402195  83 EC 0C                 sub  esp, 0Ch
004021D1  56                       push esi
004021D2  8B 74 24 14              mov  esi, dword ptr [esp+14h]
004021D6  56                       push esi
004021D7  68 4C 82 46 00           push 0046824Ch
004021DC  68 C8 F3 46 00           push 0046F3C8h
004021E1  C7 05 DC F5 46 00 00 00  mov  dword ptr [0046F5DC], 0
          00 00
004021EB  E8 83 F8 04 00           call 00451A73h
0040220D  56                       push esi
0040220E  68 30 82 46 00           push 00468230h ; "..\\MultiMedia\\MAP\\%s.Dat"
00402213  68 C8 F3 46 00           push 0046F3C8h
00402218  E8 56 F8 04 00           call 00451A73h
0040223E  68 A4 00 00 00           push 0A4h
00402243  68 C8 81 46 00           push 004681C8h
00402248  68 00 80 00 00           push 8000h
0040224D  68 C8 F3 46 00           push 0046F3C8h
00402252  E8 99 93 04 00           call 0044B5F0h
0040225C  A3 D8 F5 46 00           mov  [0046F5D8], eax ; Map容器句柄
00402266  A1 D8 F5 46 00           mov  eax, [0046F5D8]
0040226B  8D 54 24 20              lea  edx, [esp+20h]
0040226F  6A 0A                    push 0Ah
00402271  52                       push edx
00402272  50                       push eax
00402273  E8 58 9D 05 00           call 0045BFD0h ; 读取10字节
00402278  8B 4C 24 32              mov  ecx, [esp+32h] ; 头部+6 u32
0040227C  B8 A5 41 1A A4           mov  eax, 0A41A41A5h
00402281  F7 E1                    mul  ecx
00402283  2B CA                    sub  ecx, edx
00402285  D1 E9                    shr  ecx, 1
00402287  03 CA                    add  ecx, edx
00402289  C1 E9 05                 shr  ecx, 5       ; unsigned /39
0040228C  49                       dec  ecx           ; 跳过根记录
0040228D  89 0D E8 F5 46 00        mov  [0046F5E8], ecx
00402293  8D 14 89                 lea  edx, [ecx+ecx*4]
00402296  C1 E2 03                 shl  edx, 3
00402299  2B D1                    sub  edx, ecx       ; count*39
0040229B  52                       push edx
0040229C  E8 BE F3 04 00           call 0045165Fh     ; 分配索引缓存
004022A1  A3 F4 F5 46 00           mov  [0046F5F4], eax
004022A6  A1 D8 F5 46 00           mov  eax, [0046F5D8]
004022AB  6A 00                    push 0
004022AD  6A 31                    push 31h           ; 绝对偏移49=10+39
004022AF  50                       push eax
004022B0  E8 17 66 05 00           call 004588CCh     ; seek
004022B5  A1 E8 F5 46 00           mov  eax, [0046F5E8]
004022BA  8B 15 F4 F5 46 00        mov  edx, [0046F5F4]
004022C0  8D 0C 80                 lea  ecx, [eax+eax*4]
004022C3  C1 E1 03                 shl  ecx, 3
004022C6  2B C8                    sub  ecx, eax       ; count*39
004022C8  A1 D8 F5 46 00           mov  eax, [0046F5D8]
004022CD  51                       push ecx
004022CE  52                       push edx
004022CF  50                       push eax
004022D0  E8 FB 9C 05 00           call 0045BFD0h     ; 读取根后全部记录

; -----------------------------------------------------------------------------
; SYS DAT初始化：0x00402370
; -----------------------------------------------------------------------------
00402370  83 EC 0C                 sub  esp, 0Ch
00402373  68 CD 00 00 00           push 0CDh
00402378  68 C8 81 46 00           push 004681C8h
0040237D  68 00 80 00 00           push 8000h
00402382  68 AC 82 46 00           push 004682ACh ; "..\\MultiMedia\\SYS\\SYS.Dat"
00402387  E8 64 92 04 00           call 0044B5F0h
00402392  A3 D0 F5 46 00           mov  [0046F5D0], eax ; SYS句柄
004023A7  8B 0D D0 F5 46 00        mov  ecx, [0046F5D0]
004023AD  8D 44 24 0C              lea  eax, [esp+0Ch]
004023B1  6A 0A                    push 0Ah
004023B3  50                       push eax
004023B4  51                       push ecx
004023B5  E8 16 9C 05 00           call 0045BFD0h ; 读取10字节
004023BA  8B 4C 24 1E              mov  ecx, [esp+1Eh] ; 头部+6
004023BE  B8 A5 41 1A A4           mov  eax, 0A41A41A5h
004023C3  F7 E1                    mul  ecx
004023C5  2B CA                    sub  ecx, edx
004023C7  D1 E9                    shr  ecx, 1
004023C9  03 CA                    add  ecx, edx
004023CB  C1 E9 05                 shr  ecx, 5
004023CE  49                       dec  ecx
004023CF  89 0D E0 F5 46 00        mov  [0046F5E0], ecx ; SYS根后记录数
004023D5  8D 14 89                 lea  edx, [ecx+ecx*4]
004023D8  C1 E2 03                 shl  edx, 3
004023DB  2B D1                    sub  edx, ecx       ; count*39
004023DD  52                       push edx
004023DE  E8 7C F2 04 00           call 0045165Fh
004023E3  A3 EC F5 46 00           mov  [0046F5EC], eax ; SYS索引缓存
004023E8  A1 D0 F5 46 00           mov  eax, [0046F5D0]
004023ED  6A 00                    push 0
004023EF  6A 31                    push 31h
004023F1  50                       push eax
004023F2  E8 D5 64 05 00           call 004588CCh

; -----------------------------------------------------------------------------
; Music DAT初始化：0x00402433
; -----------------------------------------------------------------------------
00402433  68 DC 00 00 00           push 0DCh
00402438  68 C8 81 46 00           push 004681C8h
0040243D  68 00 80 00 00           push 8000h
00402442  68 78 82 46 00           push 00468278h ; "..\\MultiMedia\\Music\\Music.DAT"
00402447  E8 A4 91 04 00           call 0044B5F0h
00402452  A3 D4 F5 46 00           mov  [0046F5D4], eax ; Music句柄
00402467  8B 15 D4 F5 46 00        mov  edx, [0046F5D4]
0040246D  8D 4C 24 0C              lea  ecx, [esp+0Ch]
00402471  6A 0A                    push 0Ah
00402473  51                       push ecx
00402474  52                       push edx
00402475  E8 56 9B 05 00           call 0045BFD0h ; 读取10字节
0040247A  8B 4C 24 1E              mov  ecx, [esp+1Eh] ; 头部+6
0040247E  B8 A5 41 1A A4           mov  eax, 0A41A41A5h
00402483  F7 E1                    mul  ecx
00402485  2B CA                    sub  ecx, edx
00402487  D1 E9                    shr  ecx, 1
00402489  03 CA                    add  ecx, edx
0040248B  C1 E9 05                 shr  ecx, 5
0040248E  49                       dec  ecx
0040248F  89 0D E4 F5 46 00        mov  [0046F5E4], ecx ; Music根后记录数
00402495  8D 04 89                 lea  eax, [ecx+ecx*4]
00402498  C1 E0 03                 shl  eax, 3
0040249B  2B C1                    sub  eax, ecx       ; count*39
0040249D  50                       push eax
0040249E  E8 BC F1 04 00           call 0045165Fh
004024AE  A3 F0 F5 46 00           mov  [0046F5F0], eax ; Music索引缓存

; -----------------------------------------------------------------------------
; Loader入口与四项跳表：0x004025B0 / 0x004027D0
; -----------------------------------------------------------------------------
004025B0  8B 44 24 0C              mov  eax, [esp+0Ch] ; mode
004025B4  81 EC 84 01 00 00        sub  esp, 184h
004025BA  83 F8 03                 cmp  eax, 3
004025BD  56                       push esi
004025BE  57                       push edi
004025BF  0F 87 FC 00 00 00        ja   004026C1h
004025C5  FF 24 85 D0 27 40 00     jmp  dword ptr [eax*4+004027D0h]

; 0x004027D0原始16字节，按little-endian DWORD解释：
004027D0  CC 25 40 00              dd 004025CCh ; mode 0
004027D4  27 26 40 00              dd 00402627h ; mode 1
004027D8  CF 26 40 00              dd 004026CFh ; mode 2
004027DC  4B 27 40 00              dd 0040274Bh ; mode 3

; mode 0：直接文件读取
004025CC  8B 84 24 94 01 00 00     mov  eax, [esp+194h]
004025D3  8B B4 24 90 01 00 00     mov  esi, [esp+190h]
004025DA  50                       push eax
004025DB  56                       push esi
004025DC  E8 FF 01 00 00           call 004027E0h

; mode 1：直接路径失败后进Map容器组1
00402685  56                       push esi
00402686  68 C8 F3 46 00           push 0046F3C8h
0040268B  E8 50 01 00 00           call 004027E0h
00402693  83 F8 FF                 cmp  eax, -1
00402696  75 29                    jne  004026C1h
004026B1  6A 01                    push 1
004026B3  56                       push esi
004026B4  68 C8 F3 46 00           push 0046F3C8h
004026B9  E8 92 01 00 00           call 00402850h

; mode 2：直接SYS路径失败后进容器组2
004026F1  57                       push edi
004026F2  50                       push eax
004026F3  E8 E8 00 00 00           call 004027E0h
004026FB  83 F8 FF                 cmp  eax, -1
004026FE  75 C1                    jne  004026C1h
00402700  6A 02                    push 2
00402702  57                       push edi
00402703  56                       push esi
00402704  E8 47 01 00 00           call 00402850h

; mode 3：直接Music路径失败后进容器组3
0040276D  57                       push edi
0040276E  52                       push edx
0040276F  E8 6C 00 00 00           call 004027E0h
00402777  83 F8 FF                 cmp  eax, -1
0040277A  0F 85 41 FF FF FF        jne  004026C1h
00402780  6A 03                    push 3
00402782  57                       push edi
00402783  56                       push esi
00402784  E8 C7 00 00 00           call 00402850h

; -----------------------------------------------------------------------------
; 容器组读取器绑定：0x00402850
; -----------------------------------------------------------------------------
00402850  8B 44 24 0C              mov  eax, [esp+0Ch]
00402854  56                       push esi
00402855  48                       dec  eax
00402856  0F 84 E6 00 00 00        je   00402942h ; group 1 Map
0040285C  48                       dec  eax
0040285D  74 78                    je   004028D7h ; group 2 SYS
0040285F  48                       dec  eax
00402860  0F 85 46 01 00 00        jne  004029ACh

; group 3 Music
00402866  A1 E4 F5 46 00           mov  eax, [0046F5E4] ; count
0040286B  8B 0D F0 F5 46 00        mov  ecx, [0046F5F0] ; index
00402882  E8 39 01 00 00           call 004029C0h

; group 2 SYS
004028D7  8B 15 E0 F5 46 00        mov  edx, [0046F5E0] ; count
004028DD  A1 EC F5 46 00           mov  eax, [0046F5EC] ; index
004028F3  E8 C8 00 00 00           call 004029C0h

; group 1 Map
00402942  8B 0D E8 F5 46 00        mov  ecx, [0046F5E8] ; count
00402948  8B 15 F4 F5 46 00        mov  edx, [0046F5F4] ; index
0040295F  E8 5C 00 00 00           call 004029C0h

; -----------------------------------------------------------------------------
; 39-byte记录扁平查找：0x004029C0
; -----------------------------------------------------------------------------
004029C0  53                       push ebx
004029C1  8B 5C 24 18              mov  ebx, [esp+18h] ; count
004029C5  55                       push ebp
004029C6  56                       push esi
004029C7  33 F6                    xor  esi, esi
004029C9  57                       push edi
004029CA  85 DB                    test ebx, ebx
004029CC  7E 21                    jle  004029EFh
004029CE  8B 7C 24 20              mov  edi, [esp+20h] ; first cached record
004029D2  8B 6C 24 14              mov  ebp, [esp+14h] ; requested name
004029D6  8D 47 0A                 lea  eax, [edi+0Ah] ; record+10 name
004029D9  50                       push eax
004029DA  55                       push ebp
004029DB  E8 50 A3 05 00           call 0045CD30h
004029E0  83 C4 08                 add  esp, 8
004029E3  85 C0                    test eax, eax
004029E5  74 0F                    je   004029F6h
004029E7  46                       inc  esi
004029E8  83 C7 27                 add  edi, 27h ; stride 39
004029EB  3B F3                    cmp  esi, ebx
004029ED  7C E7                    jl   004029D6h
004029EF  5F                       pop  edi
004029F0  5E                       pop  esi
004029F1  5D                       pop  ebp
004029F2  32 C0                    xor  al, al
004029F4  5B                       pop  ebx
004029F5  C3                       ret
004029F6  8B 54 24 18              mov  edx, [esp+18h]
004029FA  8B 4F 02                 mov  ecx, [edi+02h] ; size
004029FD  89 0A                    mov  [edx], ecx
004029FF  8B 47 23                 mov  eax, [edi+23h] ; absolute data offset
00402A02  8B 4C 24 1C              mov  ecx, [esp+1Ch]
00402A09  89 01                    mov  [ecx], eax
00402A0B  B0 01                    mov  al, 1
00402A0D  5B                       pop  ebx
00402A0E  C3                       ret

; -----------------------------------------------------------------------------
; ASCII不区分大小写比较：0x0045CD30，关键折叠段
; -----------------------------------------------------------------------------
0045CD30  55                       push ebp
0045CD31  8B EC                    mov  ebp, esp
0045CD36  8B 75 0C                 mov  esi, [ebp+0Ch]
0045CD39  8B 7D 08                 mov  edi, [ebp+08h]
0045CD50  8A 06                    mov  al, [esi]
0045CD52  46                       inc  esi
0045CD53  8A 27                    mov  ah, [edi]
0045CD55  47                       inc  edi
0045CD56  38 C4                    cmp  ah, al
0045CD58  74 F2                    je   0045CD4Ch
0045CD5A  2C 41                    sub  al, 41h
0045CD5C  3C 1A                    cmp  al, 1Ah
0045CD5E  1A C9                    sbb  cl, cl
0045CD60  80 E1 20                 and  cl, 20h
0045CD63  02 C1                    add  al, cl
0045CD65  04 41                    add  al, 41h
0045CD67  86 E0                    xchg al, ah
0045CD69  2C 41                    sub  al, 41h
0045CD6B  3C 1A                    cmp  al, 1Ah
0045CD6D  1A C9                    sbb  cl, cl
0045CD6F  80 E1 20                 and  cl, 20h
0045CD72  02 C1                    add  al, cl
0045CD74  04 41                    add  al, 41h
0045CD76  38 E0                    cmp  al, ah
0045CD78  74 D2                    je   0045CD4Ch
0045CDB7  5B                       pop  ebx
0045CDB8  5E                       pop  esi
0045CDB9  5F                       pop  edi
0045CDBA  C9                       leave
0045CDBB  C3                       ret

