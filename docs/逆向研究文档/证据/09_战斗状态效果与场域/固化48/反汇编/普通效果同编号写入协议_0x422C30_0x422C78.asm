; 固化48：normal effect按raw ID自身编号写pending，证明0x46A668不是自身目的槽表。

/mnt/data/work48/input/RPG.exe:     file format pei-i386


Disassembly of section .text:

00422c30 <.text+0x21c30>:
  422c30:	eb 45                	jmp    0x422c77
  422c32:	8a 4b 0c             	mov    cl,BYTE PTR [ebx+0xc]
  422c35:	33 d2                	xor    edx,edx
  422c37:	88 4e 04             	mov    BYTE PTR [esi+0x4],cl
  422c3a:	8d 6b 24             	lea    ebp,[ebx+0x24]
  422c3d:	8b 7d 00             	mov    edi,DWORD PTR [ebp+0x0]
  422c40:	85 ff                	test   edi,edi
  422c42:	7e 2a                	jle    0x422c6e
  422c44:	8b 44 24 1c          	mov    eax,DWORD PTR [esp+0x1c]
  422c48:	03 c2                	add    eax,edx
  422c4a:	8b 04 86             	mov    eax,DWORD PTR [esi+eax*4]
  422c4d:	83 f8 1b             	cmp    eax,0x1b
  422c50:	7f 1c                	jg     0x422c6e
  422c52:	8b 4c 24 24          	mov    ecx,DWORD PTR [esp+0x24]
  422c56:	8b 0c 8d 94 fd 89 00 	mov    ecx,DWORD PTR [ecx*4+0x89fd94]
  422c5d:	83 bc 81 34 0a 00 00 	cmp    DWORD PTR [ecx+eax*4+0xa34],0x0
  422c64:	00 
  422c65:	7f 07                	jg     0x422c6e
  422c67:	89 bc 81 cc 00 00 00 	mov    DWORD PTR [ecx+eax*4+0xcc],edi
  422c6e:	42                   	inc    edx
  422c6f:	83 c5 04             	add    ebp,0x4
  422c72:	83 fa 05             	cmp    edx,0x5
  422c75:	7c c6                	jl     0x422c3d
  422c77:	8b             	mov    eax,DWORD PTR [esp+0x20]
