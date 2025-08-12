//#include "C:/Wichtig/System/Static/Library/WindowEngine1.0.h"
#include "/home/codeleaded/System/Static/Library/WindowEngine1.0.h"
#include "/home/codeleaded/System/Static/Library/Random.h"
#include "/home/codeleaded/System/Static/Library/TransformedView.h"


TransformedView tv;

void Setup(AlxWindow* w){
	tv = TransformedView_New((Vec2){ GetWidth(),GetHeight() });
}
void Update(AlxWindow* w){
	TransformedView_HandlePanZoom(&tv,window.Strokes,GetMouse());
	Rect r = TransformedView_Rect(&tv,GetScreenRect());

	
	Clear(BLACK);

	
	//String str = String_Format("MI:%d",nMaxIterations);
	//CStr_RenderSizeAlxFont(WINDOW_STD_ARGS,&window.AlxFont,str.Memory,str.size,0.0f,0.0f,WHITE);
	//String_Free(&str);
}
void Delete(AlxWindow* w){
	
}

int main(){
    if(Create("A* Path Finder",1200,1200,1,1,Setup,Update,Delete))
        Start();
    return 0;
}