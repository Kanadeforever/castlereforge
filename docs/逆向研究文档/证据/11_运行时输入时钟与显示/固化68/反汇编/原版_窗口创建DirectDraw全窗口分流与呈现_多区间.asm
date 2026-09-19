; 固化68补充直接反汇编证据。由当前工具从已校验输入只读生成。
; 业务结论仍以机器JSON和完整接档说明中的证据边界为准。

; ===== 窗口创建 0x00401820..0x00401880 =====

/workspace/scratch/3f7398360f84/work/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

00401820 <.text+0x820>:
  401820:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  401824:	56                   	push   esi
  401825:	6a 00                	push   0x0
  401827:	50                   	push   eax
  401828:	6a 00                	push   0x0
  40182a:	6a 00                	push   0x0
  40182c:	68 e0 01 00 00       	push   0x1e0
  401831:	68 80 02 00 00       	push   0x280
  401836:	6a 00                	push   0x0
  401838:	6a 00                	push   0x0
  40183a:	68 00 00 00 80       	push   0x80000000
  40183f:	68 18 81 46 00       	push   0x468118
  401844:	68 10 81 46 00       	push   0x468110
  401849:	68 00 00 04 00       	push   0x40000
  40184e:	a3 88 f3 46 00       	mov    ds:0x46f388,eax
  401853:	ff 15 d8 01 46 00    	call   DWORD PTR ds:0x4601d8
  401859:	8b f0                	mov    esi,eax
  40185b:	85 f6                	test   esi,esi
  40185d:	75 02                	jne    0x401861
  40185f:	5e                   	pop    esi
  401860:	c3                   	ret
  401861:	8b 44 24 0c          	mov    eax,DWORD PTR [esp+0xc]
  401865:	89 35 94 f3 46 00    	mov    DWORD PTR ds:0x46f394,esi
  40186b:	50                   	push   eax
  40186c:	56                   	push   esi
  40186d:	ff 15 d4 01 46 00    	call   DWORD PTR ds:0x4601d4
  401873:	56                   	push   esi
  401874:	ff 15 c4 01 46 00    	call   DWORD PTR ds:0x4601c4
  40187a:	6a 00                	push   0x0
  40187c:	ff                   	.byte 0xff
  40187d:	15                   	.byte 0x15
  40187e:	cc                   	int3
  40187f:	01                   	.byte 0x1


; ===== 表面描述符 0x00405B30..0x00405BC8 =====

/workspace/scratch/3f7398360f84/work/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

00405b30 <.text+0x4b30>:
  405b30:	81 ec 80 00 00 00    	sub    esp,0x80
  405b36:	8b d1                	mov    edx,ecx
  405b38:	57                   	push   edi
  405b39:	b9 1f 00 00 00       	mov    ecx,0x1f
  405b3e:	33 c0                	xor    eax,eax
  405b40:	8d 7c 24 08          	lea    edi,[esp+0x8]
  405b44:	f3 ab                	rep stos DWORD PTR es:[edi],eax
  405b46:	8b 84 24 90 00 00 00 	mov    eax,DWORD PTR [esp+0x90]
  405b4d:	c7 44 24 08 7c 00 00 	mov    DWORD PTR [esp+0x8],0x7c
  405b54:	00 
  405b55:	85 c0                	test   eax,eax
  405b57:	c7 44 24 0c 07 00 01 	mov    DWORD PTR [esp+0xc],0x10007
  405b5e:	00 
  405b5f:	c7 44 24 70 40 00 00 	mov    DWORD PTR [esp+0x70],0x40
  405b66:	00 
  405b67:	5f                   	pop    edi
  405b68:	74 08                	je     0x405b72
  405b6a:	c7 44 24 6c 40 08 00 	mov    DWORD PTR [esp+0x6c],0x840
  405b71:	00 
  405b72:	8b 8c 24 84 00 00 00 	mov    ecx,DWORD PTR [esp+0x84]
  405b79:	8b 84 24 88 00 00 00 	mov    eax,DWORD PTR [esp+0x88]
  405b80:	89 4c 24 10          	mov    DWORD PTR [esp+0x10],ecx
  405b84:	8d 4c 24 00          	lea    ecx,[esp+0x0]
  405b88:	89 44 24 0c          	mov    DWORD PTR [esp+0xc],eax
  405b8c:	8b 02                	mov    eax,DWORD PTR [edx]
  405b8e:	6a 00                	push   0x0
  405b90:	51                   	push   ecx
  405b91:	8d 4c 24 0c          	lea    ecx,[esp+0xc]
  405b95:	c7 44 24 4c 00 00 00 	mov    DWORD PTR [esp+0x4c],0x0
  405b9c:	00 
  405b9d:	c7 44 24 50 00 00 00 	mov    DWORD PTR [esp+0x50],0x0
  405ba4:	00 
  405ba5:	8b 10                	mov    edx,DWORD PTR [eax]
  405ba7:	51                   	push   ecx
  405ba8:	50                   	push   eax
  405ba9:	ff 52 18             	call   DWORD PTR [edx+0x18]
  405bac:	85 c0                	test   eax,eax
  405bae:	74 0b                	je     0x405bbb
  405bb0:	33 c0                	xor    eax,eax
  405bb2:	81 c4 80 00 00 00    	add    esp,0x80
  405bb8:	c2 0c 00             	ret    0xc
  405bbb:	8b 44 24 00          	mov    eax,DWORD PTR [esp+0x0]
  405bbf:	81 c4 80 00 00 00    	add    esp,0x80
  405bc5:	c2 0c 00             	ret    0xc


; ===== 全屏与窗口初始化分流 0x00405BD0..0x00405CA0 =====

/workspace/scratch/3f7398360f84/work/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

00405bd0 <.text+0x4bd0>:
  405bd0:	55                   	push   ebp
  405bd1:	8b ec                	mov    ebp,esp
  405bd3:	6a ff                	push   0xffffffff
  405bd5:	68 00 d0 45 00       	push   0x45d000
  405bda:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  405be0:	50                   	push   eax
  405be1:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  405be8:	81 ec 6c 01 00 00    	sub    esp,0x16c
  405bee:	53                   	push   ebx
  405bef:	56                   	push   esi
  405bf0:	8b f1                	mov    esi,ecx
  405bf2:	57                   	push   edi
  405bf3:	89 65 f0             	mov    DWORD PTR [ebp-0x10],esp
  405bf6:	8d 45 ec             	lea    eax,[ebp-0x14]
  405bf9:	8b 4e 24             	mov    ecx,DWORD PTR [esi+0x24]
  405bfc:	6a 00                	push   0x0
  405bfe:	50                   	push   eax
  405bff:	89 75 ac             	mov    DWORD PTR [ebp-0x54],esi
  405c02:	8b 51 04             	mov    edx,DWORD PTR [ecx+0x4]
  405c05:	c7 45 fc 00 00 00 00 	mov    DWORD PTR [ebp-0x4],0x0
  405c0c:	52                   	push   edx
  405c0d:	e8 54 5b 04 00       	call   0x44b766
  405c12:	85 c0                	test   eax,eax
  405c14:	7d 15                	jge    0x405c2b
  405c16:	8d 45 98             	lea    eax,[ebp-0x68]
  405c19:	68 d8 37 46 00       	push   0x4637d8
  405c1e:	50                   	push   eax
  405c1f:	c7 45 98 0c 8a 46 00 	mov    DWORD PTR [ebp-0x68],0x468a0c
  405c26:	e8 9c bf 04 00       	call   0x451bc7
  405c2b:	8b 45 ec             	mov    eax,DWORD PTR [ebp-0x14]
  405c2e:	56                   	push   esi
  405c2f:	68 58 12 46 00       	push   0x461258
  405c34:	50                   	push   eax
  405c35:	8b 08                	mov    ecx,DWORD PTR [eax]
  405c37:	ff 11                	call   DWORD PTR [ecx]
  405c39:	85 c0                	test   eax,eax
  405c3b:	7d 15                	jge    0x405c52
  405c3d:	8d 55 e0             	lea    edx,[ebp-0x20]
  405c40:	68 d8 37 46 00       	push   0x4637d8
  405c45:	52                   	push   edx
  405c46:	c7 45 e0 e4 89 46 00 	mov    DWORD PTR [ebp-0x20],0x4689e4
  405c4d:	e8 75 bf 04 00       	call   0x451bc7
  405c52:	8b 45 ec             	mov    eax,DWORD PTR [ebp-0x14]
  405c55:	50                   	push   eax
  405c56:	8b 08                	mov    ecx,DWORD PTR [eax]
  405c58:	ff 51 08             	call   DWORD PTR [ecx+0x8]
  405c5b:	8a 46 30             	mov    al,BYTE PTR [esi+0x30]
  405c5e:	84 c0                	test   al,al
  405c60:	0f 84 3e 02 00 00    	je     0x405ea4
  405c66:	8b 46 2c             	mov    eax,DWORD PTR [esi+0x2c]
  405c69:	85 c0                	test   eax,eax
  405c6b:	75 13                	jne    0x405c80
  405c6d:	8a 56 38             	mov    dl,BYTE PTR [esi+0x38]
  405c70:	f6 da                	neg    dl
  405c72:	1b d2                	sbb    edx,edx
  405c74:	81 e2 00 08 00 00    	and    edx,0x800
  405c7a:	83 c2 13             	add    edx,0x13
  405c7d:	89 56 2c             	mov    DWORD PTR [esi+0x2c],edx
  405c80:	8b 56 2c             	mov    edx,DWORD PTR [esi+0x2c]
  405c83:	8b 06                	mov    eax,DWORD PTR [esi]
  405c85:	52                   	push   edx
  405c86:	8b 15 84 f3 46 00    	mov    edx,DWORD PTR ds:0x46f384
  405c8c:	8b 08                	mov    ecx,DWORD PTR [eax]
  405c8e:	52                   	push   edx
  405c8f:	50                   	push   eax
  405c90:	ff 51 50             	call   DWORD PTR [ecx+0x50]
  405c93:	85 c0                	test   eax,eax
  405c95:	7d 15                	jge    0x405cac
  405c97:	8d 45 b0             	lea    eax,[ebp-0x50]
  405c9a:	68 d8 37 46 00       	push   0x4637d8
  405c9f:	50                   	push   eax


; ===== 窗口初始化 0x00405EA4..0x004060A0 =====

/workspace/scratch/3f7398360f84/work/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

00405ea4 <.text+0x4ea4>:
  405ea4:	8b 46 28             	mov    eax,DWORD PTR [esi+0x28]
  405ea7:	8b 48 08             	mov    ecx,DWORD PTR [eax+0x8]
  405eaa:	8b 50 04             	mov    edx,DWORD PTR [eax+0x4]
  405ead:	51                   	push   ecx
  405eae:	52                   	push   edx
  405eaf:	6a 00                	push   0x0
  405eb1:	8d 45 88             	lea    eax,[ebp-0x78]
  405eb4:	6a 00                	push   0x0
  405eb6:	50                   	push   eax
  405eb7:	ff 15 f4 01 46 00    	call   DWORD PTR ds:0x4601f4
  405ebd:	8b 0d 84 f3 46 00    	mov    ecx,DWORD PTR ds:0x46f384
  405ec3:	8b 3d ec 01 46 00    	mov    edi,DWORD PTR ds:0x4601ec
  405ec9:	6a ec                	push   0xffffffec
  405ecb:	51                   	push   ecx
  405ecc:	ff d7                	call   edi
  405ece:	8b 15 84 f3 46 00    	mov    edx,DWORD PTR ds:0x46f384
  405ed4:	50                   	push   eax
  405ed5:	52                   	push   edx
  405ed6:	ff 15 fc 01 46 00    	call   DWORD PTR ds:0x4601fc
  405edc:	f7 d8                	neg    eax
  405ede:	1b c0                	sbb    eax,eax
  405ee0:	f7 d8                	neg    eax
  405ee2:	50                   	push   eax
  405ee3:	a1 84 f3 46 00       	mov    eax,ds:0x46f384
  405ee8:	6a f0                	push   0xfffffff0
  405eea:	50                   	push   eax
  405eeb:	ff d7                	call   edi
  405eed:	8d 4d 88             	lea    ecx,[ebp-0x78]
  405ef0:	50                   	push   eax
  405ef1:	51                   	push   ecx
  405ef2:	ff 15 00 02 46 00    	call   DWORD PTR ds:0x460200
  405ef8:	8b 55 94             	mov    edx,DWORD PTR [ebp-0x6c]
  405efb:	8b 5d 8c             	mov    ebx,DWORD PTR [ebp-0x74]
  405efe:	8b 45 90             	mov    eax,DWORD PTR [ebp-0x70]
  405f01:	2b d3                	sub    edx,ebx
  405f03:	8b 0d 84 f3 46 00    	mov    ecx,DWORD PTR ds:0x46f384
  405f09:	6a 01                	push   0x1
  405f0b:	52                   	push   edx
  405f0c:	8b 55 88             	mov    edx,DWORD PTR [ebp-0x78]
  405f0f:	2b c2                	sub    eax,edx
  405f11:	50                   	push   eax
  405f12:	6a 00                	push   0x0
  405f14:	6a 00                	push   0x0
  405f16:	51                   	push   ecx
  405f17:	ff 15 f8 01 46 00    	call   DWORD PTR ds:0x4601f8
  405f1d:	8b 46 2c             	mov    eax,DWORD PTR [esi+0x2c]
  405f20:	85 c0                	test   eax,eax
  405f22:	75 13                	jne    0x405f37
  405f24:	8a 56 38             	mov    dl,BYTE PTR [esi+0x38]
  405f27:	f6 da                	neg    dl
  405f29:	1b d2                	sbb    edx,edx
  405f2b:	81 e2 00 08 00 00    	and    edx,0x800
  405f31:	83 c2 08             	add    edx,0x8
  405f34:	89 56 2c             	mov    DWORD PTR [esi+0x2c],edx
  405f37:	8b 56 2c             	mov    edx,DWORD PTR [esi+0x2c]
  405f3a:	8b 06                	mov    eax,DWORD PTR [esi]
  405f3c:	52                   	push   edx
  405f3d:	8b 15 84 f3 46 00    	mov    edx,DWORD PTR ds:0x46f384
  405f43:	8b 08                	mov    ecx,DWORD PTR [eax]
  405f45:	52                   	push   edx
  405f46:	50                   	push   eax
  405f47:	ff 51 50             	call   DWORD PTR [ecx+0x50]
  405f4a:	85 c0                	test   eax,eax
  405f4c:	7d 15                	jge    0x405f63
  405f4e:	8d 45 a0             	lea    eax,[ebp-0x60]
  405f51:	68 d8 37 46 00       	push   0x4637d8
  405f56:	50                   	push   eax
  405f57:	c7 45 a0 7c 88 46 00 	mov    DWORD PTR [ebp-0x60],0x46887c
  405f5e:	e8 64 bc 04 00       	call   0x451bc7
  405f63:	b9 1f 00 00 00       	mov    ecx,0x1f
  405f68:	33 c0                	xor    eax,eax
  405f6a:	8d bd 04 ff ff ff    	lea    edi,[ebp-0xfc]
  405f70:	f3 ab                	rep stos DWORD PTR es:[edi],eax
  405f72:	8a 46 38             	mov    al,BYTE PTR [esi+0x38]
  405f75:	c7 85 04 ff ff ff 7c 	mov    DWORD PTR [ebp-0xfc],0x7c
  405f7c:	00 00 00 
  405f7f:	84 c0                	test   al,al
  405f81:	c7 85 08 ff ff ff 01 	mov    DWORD PTR [ebp-0xf8],0x1
  405f88:	00 00 00 
  405f8b:	c7 85 6c ff ff ff 00 	mov    DWORD PTR [ebp-0x94],0x200
  405f92:	02 00 00 
  405f95:	74 0a                	je     0x405fa1
  405f97:	c7 85 6c ff ff ff 00 	mov    DWORD PTR [ebp-0x94],0x2200
  405f9e:	22 00 00 
  405fa1:	8b 06                	mov    eax,DWORD PTR [esi]
  405fa3:	8d 5e 04             	lea    ebx,[esi+0x4]
  405fa6:	6a 00                	push   0x0
  405fa8:	8d 95 04 ff ff ff    	lea    edx,[ebp-0xfc]
  405fae:	8b 08                	mov    ecx,DWORD PTR [eax]
  405fb0:	53                   	push   ebx
  405fb1:	52                   	push   edx
  405fb2:	50                   	push   eax
  405fb3:	ff 51 18             	call   DWORD PTR [ecx+0x18]
  405fb6:	85 c0                	test   eax,eax
  405fb8:	7d 15                	jge    0x405fcf
  405fba:	8d 45 b8             	lea    eax,[ebp-0x48]
  405fbd:	68 d8 37 46 00       	push   0x4637d8
  405fc2:	50                   	push   eax
  405fc3:	c7 45 b8 30 89 46 00 	mov    DWORD PTR [ebp-0x48],0x468930
  405fca:	e8 f8 bb 04 00       	call   0x451bc7
  405fcf:	8b 46 28             	mov    eax,DWORD PTR [esi+0x28]
  405fd2:	6a 00                	push   0x0
  405fd4:	8b 48 08             	mov    ecx,DWORD PTR [eax+0x8]
  405fd7:	8b 50 04             	mov    edx,DWORD PTR [eax+0x4]
  405fda:	51                   	push   ecx
  405fdb:	52                   	push   edx
  405fdc:	8b ce                	mov    ecx,esi
  405fde:	e8 4d fb ff ff       	call   0x405b30
  405fe3:	85 c0                	test   eax,eax
  405fe5:	89 46 08             	mov    DWORD PTR [esi+0x8],eax
  405fe8:	75 15                	jne    0x405fff
  405fea:	8d 45 e8             	lea    eax,[ebp-0x18]
  405fed:	68 d8 37 46 00       	push   0x4637d8
  405ff2:	50                   	push   eax
  405ff3:	c7 45 e8 54 88 46 00 	mov    DWORD PTR [ebp-0x18],0x468854
  405ffa:	e8 c8 bb 04 00       	call   0x451bc7
  405fff:	8b 06                	mov    eax,DWORD PTR [esi]
  406001:	8d 7e 10             	lea    edi,[esi+0x10]
  406004:	6a 00                	push   0x0
  406006:	57                   	push   edi
  406007:	8b 08                	mov    ecx,DWORD PTR [eax]
  406009:	6a 00                	push   0x0
  40600b:	50                   	push   eax
  40600c:	ff 51 10             	call   DWORD PTR [ecx+0x10]
  40600f:	85 c0                	test   eax,eax
  406011:	7d 15                	jge    0x406028
  406013:	8d 55 e4             	lea    edx,[ebp-0x1c]
  406016:	68 d8 37 46 00       	push   0x4637d8
  40601b:	52                   	push   edx
  40601c:	c7 45 e4 34 88 46 00 	mov    DWORD PTR [ebp-0x1c],0x468834
  406023:	e8 9f bb 04 00       	call   0x451bc7
  406028:	8b 07                	mov    eax,DWORD PTR [edi]
  40602a:	8b 15 84 f3 46 00    	mov    edx,DWORD PTR ds:0x46f384
  406030:	52                   	push   edx
  406031:	6a 00                	push   0x0
  406033:	8b 08                	mov    ecx,DWORD PTR [eax]
  406035:	50                   	push   eax
  406036:	ff 51 20             	call   DWORD PTR [ecx+0x20]
  406039:	85 c0                	test   eax,eax
  40603b:	7d 15                	jge    0x406052
  40603d:	8d 45 dc             	lea    eax,[ebp-0x24]
  406040:	68 d8 37 46 00       	push   0x4637d8
  406045:	50                   	push   eax
  406046:	c7 45 dc 0c 88 46 00 	mov    DWORD PTR [ebp-0x24],0x46880c
  40604d:	e8 75 bb 04 00       	call   0x451bc7
  406052:	8b 03                	mov    eax,DWORD PTR [ebx]
  406054:	8b 17                	mov    edx,DWORD PTR [edi]
  406056:	52                   	push   edx
  406057:	50                   	push   eax
  406058:	8b 08                	mov    ecx,DWORD PTR [eax]
  40605a:	ff 51 70             	call   DWORD PTR [ecx+0x70]
  40605d:	85 c0                	test   eax,eax
  40605f:	7d 15                	jge    0x406076
  406061:	8d 45 d4             	lea    eax,[ebp-0x2c]
  406064:	68 d8 37 46 00       	push   0x4637d8
  406069:	50                   	push   eax
  40606a:	c7 45 d4 ec 87 46 00 	mov    DWORD PTR [ebp-0x2c],0x4687ec
  406071:	e8 51 bb 04 00       	call   0x451bc7
  406076:	8b 07                	mov    eax,DWORD PTR [edi]
  406078:	50                   	push   eax
  406079:	8b 08                	mov    ecx,DWORD PTR [eax]
  40607b:	ff 51 08             	call   DWORD PTR [ecx+0x8]
  40607e:	c7 07 00 00 00 00    	mov    DWORD PTR [edi],0x0
  406084:	8b 46 28             	mov    eax,DWORD PTR [esi+0x28]
  406087:	6a 00                	push   0x0
  406089:	8b ce                	mov    ecx,esi
  40608b:	8b 50 08             	mov    edx,DWORD PTR [eax+0x8]
  40608e:	8b 40 04             	mov    eax,DWORD PTR [eax+0x4]
  406091:	52                   	push   edx
  406092:	50                   	push   eax
  406093:	e8 98 fa ff ff       	call   0x405b30
  406098:	85 c0                	test   eax,eax
  40609a:	89 46 14             	mov    DWORD PTR [esi+0x14],eax
  40609d:	75 15                	jne    0x4060b4
  40609f:	8d                   	.byte 0x8d


; ===== Flip与Blt呈现 0x004064E0..0x00406589 =====

/workspace/scratch/3f7398360f84/work/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

004064e0 <.text+0x54e0>:
  4064e0:	83 ec 14             	sub    esp,0x14
  4064e3:	56                   	push   esi
  4064e4:	8b f1                	mov    esi,ecx
  4064e6:	8a 46 30             	mov    al,BYTE PTR [esi+0x30]
  4064e9:	84 c0                	test   al,al
  4064eb:	74 2c                	je     0x406519
  4064ed:	8b 76 04             	mov    esi,DWORD PTR [esi+0x4]
  4064f0:	6a 01                	push   0x1
  4064f2:	6a 00                	push   0x0
  4064f4:	56                   	push   esi
  4064f5:	8b 06                	mov    eax,DWORD PTR [esi]
  4064f7:	ff 50 2c             	call   DWORD PTR [eax+0x2c]
  4064fa:	85 c0                	test   eax,eax
  4064fc:	0f 8d 82 00 00 00    	jge    0x406584
  406502:	8d 4c 24 04          	lea    ecx,[esp+0x4]
  406506:	68 d8 37 46 00       	push   0x4637d8
  40650b:	51                   	push   ecx
  40650c:	c7 44 24 0c b0 8a 46 	mov    DWORD PTR [esp+0xc],0x468ab0
  406513:	00 
  406514:	e8 ae b6 04 00       	call   0x451bc7
  406519:	a1 84 f3 46 00       	mov    eax,ds:0x46f384
  40651e:	8d 54 24 08          	lea    edx,[esp+0x8]
  406522:	57                   	push   edi
  406523:	52                   	push   edx
  406524:	50                   	push   eax
  406525:	ff 15 e8 01 46 00    	call   DWORD PTR ds:0x4601e8
  40652b:	8b 15 84 f3 46 00    	mov    edx,DWORD PTR ds:0x46f384
  406531:	8b 3d f0 01 46 00    	mov    edi,DWORD PTR ds:0x4601f0
  406537:	8d 4c 24 0c          	lea    ecx,[esp+0xc]
  40653b:	51                   	push   ecx
  40653c:	52                   	push   edx
  40653d:	ff d7                	call   edi
  40653f:	8b 0d 84 f3 46 00    	mov    ecx,DWORD PTR ds:0x46f384
  406545:	8d 44 24 14          	lea    eax,[esp+0x14]
  406549:	50                   	push   eax
  40654a:	51                   	push   ecx
  40654b:	ff d7                	call   edi
  40654d:	8b 4e 08             	mov    ecx,DWORD PTR [esi+0x8]
  406550:	8b 46 04             	mov    eax,DWORD PTR [esi+0x4]
  406553:	6a 00                	push   0x0
  406555:	68 00 00 00 01       	push   0x1000000
  40655a:	8b 10                	mov    edx,DWORD PTR [eax]
  40655c:	6a 00                	push   0x0
  40655e:	51                   	push   ecx
  40655f:	8d 4c 24 1c          	lea    ecx,[esp+0x1c]
  406563:	51                   	push   ecx
  406564:	50                   	push   eax
  406565:	ff 52 14             	call   DWORD PTR [edx+0x14]
  406568:	85 c0                	test   eax,eax
  40656a:	5f                   	pop    edi
  40656b:	7d 17                	jge    0x406584
  40656d:	8d 54 24 04          	lea    edx,[esp+0x4]
  406571:	68 d8 37 46 00       	push   0x4637d8
  406576:	52                   	push   edx
  406577:	c7 44 24 0c 98 8a 46 	mov    DWORD PTR [esp+0xc],0x468a98
  40657e:	00 
  40657f:	e8 43 b6 04 00       	call   0x451bc7
  406584:	5e                   	pop    esi
  406585:	83 c4 14             	add    esp,0x14
  406588:	c3                   	ret

