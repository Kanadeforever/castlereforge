  441f6f:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  441f75:	8b 41 3c             	mov    eax,DWORD PTR [ecx+0x3c]
  441f78:	85 c0                	test   eax,eax
  441f7a:	0f 8e ab 00 00 00    	jle    0x44202b
  441f80:	53                   	push   ebx
  441f81:	55                   	push   ebp
  441f82:	56                   	push   esi
  441f83:	57                   	push   edi
  441f84:	c7 44 24 10 68 00 00 	mov    DWORD PTR [esp+0x10],0x68
  441f8b:	00 
  441f8c:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  441f90:	8b 1c 08             	mov    ebx,DWORD PTR [eax+ecx*1]
  441f93:	85 db                	test   ebx,ebx
  441f95:	7e 71                	jle    0x442008
  441f97:	83 fb 09             	cmp    ebx,0x9
  441f9a:	7d 6c                	jge    0x442008
  441f9c:	8b 54 24 1c          	mov    edx,DWORD PTR [esp+0x1c]
  441fa0:	c7 44 24 14 08 00 00 	mov    DWORD PTR [esp+0x14],0x8
  441fa7:	00 
  441fa8:	8d aa 80 7b 00 00    	lea    ebp,[edx+0x7b80]
  441fae:	8b 45 d0             	mov    eax,DWORD PTR [ebp-0x30]
  441fb1:	8d 75 d0             	lea    esi,[ebp-0x30]
  441fb4:	3b c3                	cmp    eax,ebx
  441fb6:	75 3f                	jne    0x441ff7
  441fb8:	8d 8d 84 f7 ff ff    	lea    ecx,[ebp-0x87c]
  441fbe:	e8 8d 15 fe ff       	call   0x423550
  441fc3:	83 7d 00 00          	cmp    DWORD PTR [ebp+0x0],0x0
  441fc7:	7f 07                	jg     0x441fd0
  441fc9:	c7 45 00 01 00 00 00 	mov    DWORD PTR [ebp+0x0],0x1
  441fd0:	8d 04 5b             	lea    eax,[ebx+ebx*2]
  441fd3:	8d 3c c0             	lea    edi,[eax+eax*8]
  441fd6:	a1 48 1c 8e 00       	mov    eax,ds:0x8e1c48
  441fdb:	d1 e7                	shl    edi,1
  441fdd:	8b 88 a4 00 00 00    	mov    ecx,DWORD PTR [eax+0xa4]
  441fe3:	2b fb                	sub    edi,ebx
  441fe5:	c1 e7 04             	shl    edi,0x4
  441fe8:	03 f9                	add    edi,ecx
  441fea:	b9 d4 00 00 00       	mov    ecx,0xd4
  441fef:	f3 a5                	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]
  441ff1:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  441ff7:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  441ffb:	81 c5 e4 0d 00 00    	add    ebp,0xde4
  442001:	48                   	dec    eax
  442002:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  442006:	75 a6                	jne    0x441fae
  442008:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
  44200c:	8b 74 24 10          	mov    esi,DWORD PTR [esp+0x10]
  442010:	8b 51 3c             	mov    edx,DWORD PTR [ecx+0x3c]
  442013:	40                   	inc    eax
  442014:	83 c6 04             	add    esi,0x4
  442017:	3b c2                	cmp    eax,edx
  442019:	89 44 24 18          	mov    DWORD PTR [esp+0x18],eax
  44201d:	89 74 24 10          	mov    DWORD PTR [esp+0x10],esi
  442021:	0f 8c 65 ff ff ff    	jl     0x441f8c
  442027:	5f                   	pop    edi
  442028:	5e                   	pop    esi
  442029:	5d                   	pop    ebp
  44202a:	5b                   	pop    ebx
  44202b:	e8 50 8e ff ff       	call   0x43ae80
  442030:	83 c4 10             	add    esp,0x10
  442033:	c3                   	ret
  442034:	90                   	nop
  442035:	90                   	nop
  442036:	90                   	nop
  442037:	90                   	nop
  442038:	90                   	nop
  442039:	90                   	nop
  44203a:	90                   	nop
  44203b:	90                   	nop
  44203c:	90                   	nop
  44203d:	90                   	nop
  44203e:	90                   	nop
  44203f:	90                   	nop
  442040:	6a ff                	push   0xffffffff
  442042:	68 9e f9 45 00       	push   0x45f99e
  442047:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  44204d:	50                   	push   eax
  44204e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  442055:	83 ec 70             	sub    esp,0x70
  442058:	53                   	push   ebx
  442059:	55                   	push   ebp
  44205a:	56                   	push   esi
  44205b:	8b e9                	mov    ebp,ecx
  44205d:	33 db                	xor    ebx,ebx
  44205f:	57                   	push   edi
  442060:	53                   	push   ebx
  442061:	8d 4c 24 20          	lea    ecx,[esp+0x20]
  442065:	89 5c 24 18          	mov    DWORD PTR [esp+0x18],ebx
  442069:	e8 82 f6 ff ff       	call   0x4416f0
  44206e:	53                   	push   ebx
  44206f:	89 9c 24 8c 00 00 00 	mov    DWORD PTR [esp+0x8c],ebx
