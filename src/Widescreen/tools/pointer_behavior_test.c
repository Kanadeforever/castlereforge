/*
 * 直接编译生产widescreen.c，用假输入/Display快照测试坐标与绘图时序。
 * 这个独立宿主的地址空间不含游戏；固定地址页只是测试夹具，绝不注入正在运行的RPG。
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#undef NULL
#include "../source/platform.h"
#include "../source/game_addresses.h"
/* 仅把外部游戏地址替换为宿主自己的变量；所有被测函数、公式和拷贝循环仍直接来自生产源码。 */
static i32 test_camera_x, test_camera_y;
static Point32 test_world;
static u8* test_mouse_pointer;
static void FASTCALL fake_draw(void* mouse,void* edx);
#undef GLOBAL_MOUSE_WORLD_X
#undef GLOBAL_CAMERA_X
#undef GLOBAL_CAMERA_Y
#undef GLOBAL_MOUSE_MANAGER
#undef FN_MOUSE_DRAW
#define GLOBAL_MOUSE_WORLD_X ((SIZE_T)&test_world)
#define GLOBAL_CAMERA_X ((SIZE_T)&test_camera_x)
#define GLOBAL_CAMERA_Y ((SIZE_T)&test_camera_y)
#define GLOBAL_MOUSE_MANAGER ((SIZE_T)&test_mouse_pointer)
#define FN_MOUSE_DRAW ((SIZE_T)fake_draw)
#include "../source/widescreen.c"

static Point32 test_output;
static u32 test_flags;
static int test_draws, test_bad_geometry, checks;
static u8 test_backing[768u*576u*2u], test_backup[768u*576u*2u];
static u8 test_staging[1248u*576u*2u];
static u8 test_display[128], test_mode[32], test_mouse[640];

void Runtime_MemCopy(void* d,const void* s,SIZE_T n) { memcpy(d,s,n); }
void Runtime_MemZero(void* d,SIZE_T n) { memset(d,0,n); }
void Runtime_Log(const char* s) { (void)s; }
void Runtime_LogHex(const char* s,u32 v) { (void)s;(void)v; }
void Runtime_LogU32(const char* s,u32 v) { (void)s;(void)v; }
u32 Runtime_GetTickCountMs(void) { return 0; }
void* Runtime_Alloc(SIZE_T n) { return malloc(n); }
u32 Runtime_ReadPluginIniU32(const char* s,const char* k,u32 d,u32 a,u32 b) {
    (void)s;(void)k;(void)a;(void)b;return d;
}
int Runtime_PatchCall(u32 a,u32 b,const void* c,const char* d) { (void)a;(void)b;(void)c;(void)d;return 0; }
int Runtime_PatchPointer(u32 a,const void* b,void** c,const char* d) { (void)a;(void)b;(void)c;(void)d;return 0; }
int Runtime_RestoreCall(u32 a,u32 b,u32 c) { (void)a;(void)b;(void)c;return 0; }
int Runtime_DeclarePatch(u32 a,const u8* b,const u8* c,u32 d) { (void)a;(void)b;(void)c;(void)d;return 0; }
static BOOL WINAPI fake_cursor(Point32* p) { *p=test_output; return TRUE; }
void* Runtime_GetPointerNext(u32 slot) { return slot == IAT_GETCURSORPOS ? (void*)fake_cursor : NULL; }
static CastleResult CASTLE_RUNTIME_CALL fake_state(CastleGameStateSnapshotV1* state) {
    state->flags=test_flags;state->map_input_gate=1;state->map_key_mode=1;return CASTLE_OK;
}
static const CastleGameStateApiV1 fake_state_api = {
    CASTLE_GAME_STATE_API_MAGIC,CASTLE_SIZEOF_GAME_STATE_API_V1,1,1,fake_state,NULL,NULL,NULL
};
static void FASTCALL fake_draw(void* mouse,void* edx) {
    u32 x=*(u32*)((u8*)mouse+MOUSE_POS_X), y=*(u32*)((u8*)mouse+MOUSE_POS_Y);
    (void)edx;
    ++test_draws;
    if (!original_display_geometry_ok(test_display)) test_bad_geometry=1;
    /* 假绘制器只写一个像素，验证拷回区域与原始backing恢复，而不假装测试真实SF2效果。 */
    ((u16*)test_backing)[(y+48u)*768u+x+64u]=0xF800u;
}
#define CHECK(x) do { ++checks; if (!(x)) {printf("FAIL %d: %s\n",__LINE__,#x);return 1;} } while(0)
int main(void) {
    Point32 p;
    Point32* world=(Point32*)GLOBAL_MOUSE_WORLD_X;
    u32 width, blocked, x, i;
    *(u8**)GLOBAL_MOUSE_MANAGER=test_mouse;
    *(u8**)(test_display+DISPLAY_MODE_OBJECT)=test_mode;
    *(u8**)(test_display+DISPLAY_BACKING_PIXELS)=test_backing;
    write_u32(test_display,DISPLAY_BACKING_WIDTH,768u);write_u32(test_display,DISPLAY_BACKING_HEIGHT,576u);
    write_u32(test_display,DISPLAY_EXTRA_X,64u);write_u32(test_display,DISPLAY_EXTRA_Y,48u);
    write_u32(test_display,DISPLAY_BYTES_PER_PIXEL,2u);
    write_u32(test_mode,MODE_WIDTH,640u);write_u32(test_mode,MODE_HEIGHT,480u);
    g_backing_backup=test_backup;g_present_staging=test_staging;g_sdk_services_ready=1;
    g_sdk_game_state_api=&fake_state_api;
    *(i32*)GLOBAL_CAMERA_X=1000;*(i32*)GLOBAL_CAMERA_Y=200;
    for (i=0;i<2;++i) {
        width=i?1120u:854u;g_output_width=width;g_side_width=(width-640u)/2u;g_present_staging_w=width+128u;
        memset(&g_sdk_geometry,0,sizeof(g_sdk_geometry));
        g_sdk_geometry.output_width=width;g_sdk_geometry.output_height=480u;
        g_sdk_geometry.center_x=(i32)g_side_width;g_sdk_geometry.center_width=640;g_sdk_geometry.center_height=480;
        g_sdk_geometry.left_world_width=g_sdk_geometry.right_world_width=g_side_width;
        g_sdk_geometry.effective_camera_x=900;g_sdk_geometry.effective_camera_y=200;
        g_sdk_geometry.display_mode=CASTLE_DISPLAY_WIDE_WORLD;g_sdk_geometry.projection_scope=CASTLE_PROJECTION_FULL_OUTPUT;
        test_flags=CASTLE_GAME_FLAG_FREE_ROAM_CANDIDATE;
        for(x=0;x<width;++x) {
            test_output.x=(i32)x;test_output.y=240;
            CHECK(Hook_GetCursorPos(world));
            CHECK(world->x+1000==900+(i32)x-(i32)g_side_width);
            CHECK(Hook_GetCursorPos(&p) && p.x==(i32)x-(i32)g_side_width);
            blocked=0;CHECK(pointer_filter_button(1,(short)0x8000,&blocked)==(short)0x8000);
        }
        /* 模态出现当帧就关闭侧区，不能继续使用上一张自由探索画面的可点击范围。 */
        test_flags=CASTLE_GAME_FLAG_BATTLE_ACTIVE;test_output.x=0;blocked=0;
        CHECK(pointer_filter_button(1,(short)0x8000,&blocked)==0);
        test_output.x=(i32)g_side_width+320;
        CHECK(pointer_filter_button(1,(short)0x8000,&blocked)==0);
        CHECK(pointer_filter_button(1,0,&blocked)==0);
        CHECK(pointer_filter_button(1,(short)0x8000,&blocked)==(short)0x8000);
        /* 小地图左边没有实际世界；图标可经过，输入必须拒绝。 */
        test_flags=CASTLE_GAME_FLAG_FREE_ROAM_CANDIDATE;g_sdk_geometry.left_world_width=20;
        test_output.x=0;CHECK(Hook_GetCursorPos(&p) && p.x==-0x4000);
        /* 两个边缘的绘制都要保持原版行距、单次调用和backing逐字节恢复。 */
        for(x=0;x<2;++x) {
            memset(test_backing,0x11,sizeof(test_backing));memset(test_staging,0x22,sizeof(test_staging));
            test_output.x=x?(i32)width-1:0;test_output.y=240;g_output_cursor_valid=1;
            test_draws=0;test_bad_geometry=0;
            draw_cursor_on_present_staging(test_display);
            CHECK(test_draws==1 && !test_bad_geometry);
            CHECK(((u16*)test_staging)[(240u+48u)*g_present_staging_w+(u32)test_output.x+64u]==0xF800u);
            CHECK(memcmp(test_backing,test_backup,sizeof(test_backing))==0);
        }
    }
    printf("PASS %d pointer assertions\n",checks);
    return 0;
}
