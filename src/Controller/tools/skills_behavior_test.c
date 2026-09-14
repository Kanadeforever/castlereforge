/*
 * 技能页确认行为宿主：直接包含生产interface_skills.c，验证第一次A提交原版当前按钮。
 * 假对象只提供确认框、按钮和输入边沿，不启动游戏，也不写任何RPG固定地址。
 */
#include <stdio.h>
#include <string.h>
#undef NULL
#include "../source/platform.h"
#include "../source/game_addresses.h"

static u8* test_interface_pointer;
#undef GLOBAL_INTERFACE_UI
#define GLOBAL_INTERFACE_UI ((SIZE_T)&test_interface_pointer)
#include "../source/interface_skills.c"

static InputAction test_pressed_action=INPUT_ACTION_COUNT;
static void* test_pending_button;
static int test_pending_code;
static UiEventOwner test_pending_owner;
static int test_claims;
static int checks;
static u8 test_interface[0x700u];
static u8 test_page[0x800u];

int Runtime_PtrOk(const void* pointer) { return pointer != NULL; }
void Runtime_Log(const char* message) { (void)message; }
int Runtime_InterfaceSkillsProtocolOk(void) { return 1; }
int UiBridge_InstallInterfaceSkillsHooks(void) { return 1; }
int InputRouter_PressedOn(InputContext context,InputAction action,InputRouteLayer layer) {
    return context==INPUT_CTX_INTERFACE_SKILLS && layer==INPUT_LAYER_OVERLAY && action==test_pressed_action;
}
void InputRouter_Consume(InputAction action) { (void)action; }
int UiBridge_EventPending(void) { return test_pending_code != 0; }
UiEventOwner UiBridge_EventOwner(void) { return test_pending_owner; }
void UiBridge_RequestEventOwned(UiEventOwner owner,void* button,int code,const char* text) {
    (void)text;
    if (test_pending_code) return;
    test_pending_owner=owner;test_pending_button=button;test_pending_code=code;
}
void UiBridge_ClearEventOwned(UiEventOwner owner) {
    if (test_pending_owner==owner) {
        test_pending_owner=UI_EVENT_OWNER_NONE;test_pending_button=NULL;test_pending_code=0;
    }
}
void Cursor_ClaimForControllerNavigation(void) { ++test_claims; }
void Cursor_ShowMenuFocusAt(i32 x,i32 y) { (void)x;(void)y; }
void Cursor_HideMenuFocusImmediately(void) { }
int Cursor_ControllerOwnsPointer(void) { return 1; }
int UiBridge_GetButtonScreenCenter(void* button,i32* x,i32* y) {
    (void)button;if(x)*x=0;if(y)*y=0;return 1;
}

#define CHECK(value) do { ++checks; if (!(value)) { \
    printf("FAIL %d: %s\n",__LINE__,#value);return 1; } } while(0)

static void reset_popup(u8* popup,u8* yes_button,u8* no_button,i32 native_focus) {
    memset(&g_skills,0,sizeof(g_skills));
    g_skills.enabled=1;
    memset(test_interface,0,sizeof(test_interface));memset(test_page,0,sizeof(test_page));
    memset(popup,0,0x600u);memset(yes_button,0,0x50u);memset(no_button,0,0x50u);
    test_interface_pointer=test_interface;
    *(i32*)(test_interface+INTERFACE_STATE)=3;
    *(void**)(test_interface+INTERFACE_PAGE_3)=test_page;
    *(void**)(test_page+INTERFACE_SKILLS_POPUP)=popup;
    popup[POPUP_ACTIVE]=1u;
    yes_button[0x45u]=1u;no_button[0x45u]=1u;
    *(void**)(popup+POPUP_BUTTON_YES)=yes_button;
    *(void**)(popup+POPUP_BUTTON_NO)=no_button;
    *(i32*)(popup+POPUP_NATIVE_SELECTION)=native_focus;
    test_pressed_action=INPUT_ACTION_COUNT;
    test_pending_owner=UI_EVENT_OWNER_NONE;test_pending_button=NULL;test_pending_code=0;test_claims=0;
}

int main(void) {
    static u8 popup[0x600u],yes_button[0x50u],no_button[0x50u];

    /* 原版1表示第一个/确认；第一次A必须直接提交Yes，不能硬编码成取消。 */
    reset_popup(popup,yes_button,no_button,1);
    test_pressed_action=INPUT_CONFIRM;
    skills_update_popup(popup);
    CHECK(g_skills.popup_nav_active && g_skills.popup_focus==0);
    CHECK(test_claims==2 && test_pending_button==yes_button && test_pending_code==2);
    CHECK(test_pending_owner==UI_EVENT_OWNER_INTERFACE);
    CHECK(InterfaceSkills_FilterPopupHit(yes_button,NULL)==1);
    CHECK(InterfaceSkills_FilterPopupHit(no_button,NULL)==0);

    /* 原版0表示第二个/取消；A仍须忠实执行画面当前项，B也固定走取消。 */
    reset_popup(popup,yes_button,no_button,0);
    test_pressed_action=INPUT_CONFIRM;
    skills_update_popup(popup);
    CHECK(g_skills.popup_nav_active && g_skills.popup_focus==1);
    CHECK(test_pending_button==no_button && test_pending_code==2);

    reset_popup(popup,yes_button,no_button,1);
    test_pressed_action=INPUT_CANCEL;
    skills_update_popup(popup);
    CHECK(g_skills.popup_nav_active && g_skills.popup_focus==1);
    CHECK(test_pending_button==no_button && test_pending_code==2);

    /* 方向先选取消/确认后，第一次A同样只提交当前视觉按钮。 */
    reset_popup(popup,yes_button,no_button,1);
    test_pressed_action=INPUT_NAV_RIGHT;skills_update_popup(popup);
    test_pressed_action=INPUT_ACTION_COUNT;skills_update_popup(popup);
    test_pressed_action=INPUT_CONFIRM;skills_update_popup(popup);
    CHECK(test_pending_button==no_button);

    reset_popup(popup,yes_button,no_button,0);
    test_pressed_action=INPUT_NAV_LEFT;skills_update_popup(popup);
    test_pressed_action=INPUT_ACTION_COUNT;skills_update_popup(popup);
    test_pressed_action=INPUT_CONFIRM;skills_update_popup(popup);
    CHECK(test_pending_button==yes_button);

    printf("PASS %d skill assertions\n",checks);
    return 0;
}
