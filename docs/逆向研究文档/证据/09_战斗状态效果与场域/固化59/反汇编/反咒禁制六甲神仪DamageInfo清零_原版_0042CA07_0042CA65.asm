
/mnt/data/solid58_inputs/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

0042ca07 <.text+0x2ba07>:
  42ca07:	8b 54 24 24          	mov    edx,DWORD PTR [esp+0x24]
  42ca0b:	6a 02                	push   0x2
  42ca0d:	8b cd                	mov    ecx,ebp
  42ca0f:	8b 42 04             	mov    eax,DWORD PTR [edx+0x4]
  42ca12:	50                   	push   eax
  42ca13:	e8 28 f5 ff ff       	call   0x42bf40
  42ca18:	84 c0                	test   al,al
  42ca1a:	74 16                	je     0x42ca32
  42ca1c:	83 7f 18 02          	cmp    DWORD PTR [edi+0x18],0x2
  42ca20:	75 10                	jne    0x42ca32
  42ca22:	33 c0                	xor    eax,eax
  42ca24:	89 44 24 20          	mov    DWORD PTR [esp+0x20],eax
  42ca28:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  42ca2c:	89 46 04             	mov    DWORD PTR [esi+0x4],eax
  42ca2f:	89 46 08             	mov    DWORD PTR [esi+0x8],eax
  42ca32:	8b 4c 24 24          	mov    ecx,DWORD PTR [esp+0x24]
  42ca36:	6a 06                	push   0x6
  42ca38:	8b 51 04             	mov    edx,DWORD PTR [ecx+0x4]
  42ca3b:	8b cd                	mov    ecx,ebp
  42ca3d:	52                   	push   edx
  42ca3e:	e8 fd f4 ff ff       	call   0x42bf40
  42ca43:	84 c0                	test   al,al
  42ca45:	74 1a                	je     0x42ca61
  42ca47:	8b 4f 18             	mov    ecx,DWORD PTR [edi+0x18]
  42ca4a:	33 c0                	xor    eax,eax
  42ca4c:	3b c8                	cmp    ecx,eax
  42ca4e:	74 05                	je     0x42ca55
  42ca50:	83 f9 01             	cmp    ecx,0x1
  42ca53:	75 0c                	jne    0x42ca61
  42ca55:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  42ca59:	89 46 04             	mov    DWORD PTR [esi+0x4],eax
  42ca5c:	89 46 08             	mov    DWORD PTR [esi+0x8],eax
  42ca5f:	eb 04                	jmp    0x42ca65
  42ca61:	8b 44 24 20          	mov    eax,DWORD PTR [esp+0x20]
