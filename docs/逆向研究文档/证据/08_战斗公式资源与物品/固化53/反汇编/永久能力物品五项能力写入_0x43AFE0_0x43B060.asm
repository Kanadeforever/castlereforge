; 固化53：永久能力物品 effect-ID writer。
; effect54/55/56/57/58 分别写 +0x3C/+0x40/+0x48/+0x4C/+0x44。

/mnt/data/work53/input/RPG.exe:     file format pei-i386


Disassembly of section .text:

0043afe0 <.text+0x39fe0>:
  43afe0:	e1 2b                	loope  0x43b00d
  43afe2:	c8 8b 83 a4          	enter  0x838b,0xa4
  43afe6:	00 00                	add    BYTE PTR [eax],al
  43afe8:	00 c1                	add    cl,al
  43afea:	e1 04                	loope  0x43aff0
  43afec:	03 c8                	add    ecx,eax
  43afee:	bb 05 00 00 00       	mov    ebx,0x5
  43aff3:	8b 57 ec             	mov    edx,DWORD PTR [edi-0x14]
  43aff6:	8b 85 d0 00 00 00    	mov    eax,DWORD PTR [ebp+0xd0]
  43affc:	85 d2                	test   edx,edx
  43affe:	7e 44                	jle    0x43b044
  43b000:	8b 17                	mov    edx,DWORD PTR [edi]
  43b002:	8d 72 d5             	lea    esi,[edx-0x2b]
  43b005:	83 fe 11             	cmp    esi,0x11
  43b008:	77 3a                	ja     0x43b044
  43b00a:	33 d2                	xor    edx,edx
  43b00c:	8a 96 54 b1 43 00    	mov    dl,BYTE PTR [esi+0x43b154]
  43b012:	ff 24 95 30 b1 43 00 	jmp    DWORD PTR [edx*4+0x43b130]
  43b019:	c7 81 9c 00 00 00 01 	mov    DWORD PTR [ecx+0x9c],0x1
  43b020:	00 00 00 
  43b023:	01 41 3c             	add    DWORD PTR [ecx+0x3c],eax
  43b026:	eb 1c                	jmp    0x43b044
  43b028:	01 41 40             	add    DWORD PTR [ecx+0x40],eax
  43b02b:	eb 17                	jmp    0x43b044
  43b02d:	01 41 48             	add    DWORD PTR [ecx+0x48],eax
  43b030:	eb 12                	jmp    0x43b044
  43b032:	01 41 4c             	add    DWORD PTR [ecx+0x4c],eax
  43b035:	eb 0d                	jmp    0x43b044
  43b037:	01 41 44             	add    DWORD PTR [ecx+0x44],eax
  43b03a:	eb 08                	jmp    0x43b044
  43b03c:	01 41 2c             	add    DWORD PTR [ecx+0x2c],eax
  43b03f:	eb 03                	jmp    0x43b044
  43b041:	01 41 34             	add    DWORD PTR [ecx+0x34],eax
  43b044:	83 c7 04             	add    edi,0x4
  43b047:	4b                   	dec    ebx
  43b048:	75 a9                	jne    0x43aff3
  43b04a:	8b 7c 24 18          	mov    edi,DWORD PTR [esp+0x18]
  43b04e:	85 ff                	test   edi,edi
  43b050:	7e 48                	jle    0x43b09a
  43b052:	8b 71 2c             	mov    esi,DWORD PTR [ecx+0x2c]
  43b055:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  43b05a:	8b d6                	mov    edx,esi
  43b05c:	0f af 54 24        	imul   edx,DWORD PTR [esp+0x14]
