
/mnt/data/solid64_inputs/RPG.exe:     file format pei-i386


Disassembly of section .text:

00420f1d <.text+0x1ff1d>:
  420f1d:	bd 06 00 00 00       	mov    ebp,0x6
  420f22:	8b 08                	mov    ecx,DWORD PTR [eax]
  420f24:	8b 0c 8d 94 fd 89 00 	mov    ecx,DWORD PTR [ecx*4+0x89fd94]
  420f2b:	85 c9                	test   ecx,ecx
  420f2d:	74 49                	je     0x420f78
  420f2f:	8b 97 d0 0d 00 00    	mov    edx,DWORD PTR [edi+0xdd0]
  420f35:	85 d2                	test   edx,edx
  420f37:	7e 19                	jle    0x420f52
  420f39:	89 99 e8 0c 00 00    	mov    DWORD PTR [ecx+0xce8],ebx
  420f3f:	8b 10                	mov    edx,DWORD PTR [eax]
  420f41:	8b 0c 95 94 fd 89 00 	mov    ecx,DWORD PTR [edx*4+0x89fd94]
  420f48:	8b 17                	mov    edx,DWORD PTR [edi]
  420f4a:	89 91 e0 0d 00 00    	mov    DWORD PTR [ecx+0xde0],edx
  420f50:	eb 26                	jmp    0x420f78
  420f52:	8b 97 48 08 00 00    	mov    edx,DWORD PTR [edi+0x848]
  420f58:	85 d2                	test   edx,edx
  420f5a:	74 1c                	je     0x420f78
  420f5c:	39 6a 18             	cmp    DWORD PTR [edx+0x18],ebp
  420f5f:	74 17                	je     0x420f78
  420f61:	89 99 e8 0c 00 00    	mov    DWORD PTR [ecx+0xce8],ebx
  420f67:	8b 08                	mov    ecx,DWORD PTR [eax]
  420f69:	8b 14 8d 94 fd 89 00 	mov    edx,DWORD PTR [ecx*4+0x89fd94]
  420f70:	8b 0f                	mov    ecx,DWORD PTR [edi]
  420f72:	89 8a e0 0d 00 00    	mov    DWORD PTR [edx+0xde0],ecx
