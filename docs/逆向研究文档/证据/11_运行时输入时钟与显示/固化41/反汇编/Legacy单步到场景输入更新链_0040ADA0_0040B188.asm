; 固化41：selected Map runtime dispatch上层唯一链
  40ada0:	56                   	push   esi
  40ada1:	8b f1                	mov    esi,ecx
  40ada3:	57                   	push   edi
  40ada4:	33 ff                	xor    edi,edi
  40ada6:	83 3e 00             	cmp    DWORD PTR [esi],0x0
  40ada9:	7e 28                	jle    0x40add3
  40adab:	53                   	push   ebx
  40adac:	33 db                	xor    ebx,ebx
  40adae:	8b 46 04             	mov    eax,DWORD PTR [esi+0x4]
  40adb1:	8b 4e 10             	mov    ecx,DWORD PTR [esi+0x10]
  40adb4:	3b f8                	cmp    edi,eax
  40adb6:	75 09                	jne    0x40adc1
  40adb8:	03 cb                	add    ecx,ebx
  40adba:	e8 c1 e7 ff ff       	call   0x409580
  40adbf:	eb 07                	jmp    0x40adc8
  40adc1:	03 cb                	add    ecx,ebx
  40adc3:	e8 48 e7 ff ff       	call   0x409510
  40adc8:	8b 06                	mov    eax,DWORD PTR [esi]
  40adca:	47                   	inc    edi
  40adcb:	83 c3 38             	add    ebx,0x38
  40adce:	3b f8                	cmp    edi,eax
  40add0:	7c dc                	jl     0x40adae
  40add2:	5b                   	pop    ebx
  40add3:	5f                   	pop    edi
  40add4:	5e                   	pop    esi
  40add5:	c3                   	ret

  40b15f:	75 59                	jne    0x40b1ba
  40b161:	8a 86 18 02 00 00    	mov    al,BYTE PTR [esi+0x218]
  40b167:	84 c0                	test   al,al
  40b169:	74 48                	je     0x40b1b3
  40b16b:	68 b8 0b 00 00       	push   0xbb8
  40b170:	56                   	push   esi
  40b171:	8b ce                	mov    ecx,esi
  40b173:	e8 88 93 02 00       	call   0x434500
  40b178:	8a 86 19 02 00 00    	mov    al,BYTE PTR [esi+0x219]
  40b17e:	84 c0                	test   al,al
  40b180:	75 0b                	jne    0x40b18d
  40b182:	8b 8e 80 02 00 00    	mov    ecx,DWORD PTR [esi+0x280]
  40b188:	e8 13 fc ff ff       	call   0x40ada0
  40b18d:	8b 0d e0 f7 89 00    	mov    ecx,DWORD PTR ds:0x89f7e0
  40b193:	68 07 b2 01 00       	push   0x1b207
  40b198:	68 07 b2 01 00       	push   0x1b207
  40b19d:	68 07 b2 01 00       	push   0x1b207
  40b1a2:	e8 09 f1 ff ff       	call   0x40a2b0
  40b1a7:	e8 b4 01 04 00       	call   0x44b360
  40b1ac:	e8 3f ba ff ff       	call   0x406bf0
  40b1b1:	5e                   	pop    esi
