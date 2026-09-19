
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00439820 <.text+0x38820>:
  439820:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  439824:	8b 91 d8 5d 00 00    	mov    edx,DWORD PTR [ecx+0x5dd8]
  43982a:	03 d0                	add    edx,eax
  43982c:	89 91 d8 5d 00 00    	mov    DWORD PTR [ecx+0x5dd8],edx
  439832:	c2 04 00             	ret    0x4
  439835:	90                   	nop
  439836:	90                   	nop
  439837:	90                   	nop
  439838:	90                   	nop
  439839:	90                   	nop
  43983a:	90                   	nop
  43983b:	90                   	nop
  43983c:	90                   	nop
  43983d:	90                   	nop
  43983e:	90                   	nop
  43983f:	90                   	nop
  439840:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  439844:	8b 91 d8 5d 00 00    	mov    edx,DWORD PTR [ecx+0x5dd8]
  43984a:	2b d0                	sub    edx,eax
  43984c:	89 91 d8 5d 00 00    	mov    DWORD PTR [ecx+0x5dd8],edx
  439852:	79 0a                	jns    0x43985e
  439854:	c7 81 d8 5d 00 00 00 	mov    DWORD PTR [ecx+0x5dd8],0x0
  43985b:	00 00 00 
  43985e:	c2 04 00             	ret    0x4
  439861:	90                   	nop
  439862:	90                   	nop
  439863:	90                   	nop
  439864:	90                   	nop
  439865:	90                   	nop
  439866:	90                   	nop
  439867:	90                   	nop
  439868:	90                   	nop
  439869:	90                   	nop
  43986a:	90                   	nop
  43986b:	90                   	nop
  43986c:	90                   	nop
  43986d:	90                   	nop
  43986e:	90                   	nop
  43986f:	90                   	nop
  439870:	8b 81 d8 5d 00 00    	mov    eax,DWORD PTR [ecx+0x5dd8]
  439876:	c3                   	ret
