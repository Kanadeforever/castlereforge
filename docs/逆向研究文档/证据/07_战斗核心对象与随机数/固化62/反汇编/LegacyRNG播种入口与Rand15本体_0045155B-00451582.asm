
/mnt/data/solid62_inputs/RPG_original.exe:     file format pei-i386


Disassembly of section .text:

0045155b <.text+0x5055b>:
  45155b:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  45155f:	a3 9c e2 46 00       	mov    ds:0x46e29c,eax
  451564:	c3                   	ret
  451565:	a1 9c e2 46 00       	mov    eax,ds:0x46e29c
  45156a:	69 c0 fd 43 03 00    	imul   eax,eax,0x343fd
  451570:	05 c3 9e 26 00       	add    eax,0x269ec3
  451575:	a3 9c e2 46 00       	mov    ds:0x46e29c,eax
  45157a:	c1 f8 10             	sar    eax,0x10
  45157d:	25 ff 7f 00 00       	and    eax,0x7fff
  451582:	c3                   	ret
