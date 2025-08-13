//#include "C:/Wichtig/System/Static/Library/WindowEngine1.0.h"
#include "/home/codeleaded/System/Static/Library/WindowEngine1.0.h"
#include "/home/codeleaded/System/Static/Library/Random.h"
#include "/home/codeleaded/System/Static/Library/TransformedView.h"
#include "/home/codeleaded/System/Static/Library/AStar.h"

typedef struct WorldPoint2D {
	float x;
	float y;
	int t;
} WorldPoint2D;

#define MAP_WIDTH	16
#define MAP_HEIGHT	16

AStarNode* nodes;
AStarNode* start;
AStarNode* end;
TransformedView tv;


void AStarNode_UpdateNeighbours(){
	for(int i = 0;i<MAP_HEIGHT;i++){
		for(int j = 0;j<MAP_WIDTH;j++){
			AStarNode* an = nodes + (i * MAP_WIDTH + j);
			Vector_Clear(&an->neighbours);
			
			// immediate neighbours
			if(i>0){
				AStarNode* nb = nodes + ((i - 1) * MAP_WIDTH + j);
				if(!((WorldPoint2D*)nb->data)->t) Vector_Push(&an->neighbours,(AStarNode*[]){ nb });
			}
			if(i<MAP_HEIGHT - 1){
				AStarNode* nb = nodes + ((i + 1) * MAP_WIDTH + j);
				if(!((WorldPoint2D*)nb->data)->t) Vector_Push(&an->neighbours,(AStarNode*[]){ nb });
			}
			if(j>0){
				AStarNode* nb = nodes + (i * MAP_WIDTH + (j - 1));
				if(!((WorldPoint2D*)nb->data)->t) Vector_Push(&an->neighbours,(AStarNode*[]){ nb });
			}
			if(j<MAP_WIDTH - 1){
				AStarNode* nb = nodes + (i * MAP_WIDTH + (j + 1));
				if(!((WorldPoint2D*)nb->data)->t) Vector_Push(&an->neighbours,(AStarNode*[]){ nb });
			}

			// cornor neighbours
			if(i>0 && j>0){
				AStarNode* nb = nodes + ((i - 1) * MAP_WIDTH + (j - 1));
				if(!((WorldPoint2D*)nb->data)->t) Vector_Push(&an->neighbours,(AStarNode*[]){ nb });
			}
			if(i>0 && j<MAP_WIDTH - 1){
				AStarNode* nb = nodes + ((i - 1) * MAP_WIDTH + (j + 1));
				if(!((WorldPoint2D*)nb->data)->t) Vector_Push(&an->neighbours,(AStarNode*[]){ nb });
			}
			if(i<MAP_HEIGHT - 1 && j>0){
				AStarNode* nb = nodes + ((i + 1) * MAP_WIDTH + (j - 1));
				if(!((WorldPoint2D*)nb->data)->t) Vector_Push(&an->neighbours,(AStarNode*[]){ nb });
			}
			if(i<MAP_HEIGHT - 1 && j<MAP_WIDTH - 1){
				AStarNode* nb = nodes + ((i + 1) * MAP_WIDTH + (j + 1));
				if(!((WorldPoint2D*)nb->data)->t) Vector_Push(&an->neighbours,(AStarNode*[]){ nb });
			}
		}
	}
}

void Setup(AlxWindow* w){
	tv = TransformedView_New((Vec2){ GetHeight(),GetWidth() });

	nodes = (AStarNode*)malloc(sizeof(AStarNode) * MAP_WIDTH * MAP_HEIGHT);
	for(int i = 0;i<MAP_HEIGHT;i++){
		for(int j = 0;j<MAP_WIDTH;j++){
			nodes[i * MAP_WIDTH + j] = AStarNode_New((WorldPoint2D[]){ { j,i,0 } },sizeof(WorldPoint2D));
		}
	}

	start = NULL;
	end = NULL;
}
void Update(AlxWindow* w){
	TransformedView_Output(&tv,(Vec2){ GetHeight(),GetWidth() });
	TransformedView_HandlePanZoom(&tv,window.Strokes,GetMouse());
	Rect r = TransformedView_Rect(&tv,GetScreenRect());

	if(Stroke(ALX_MOUSE_L).PRESSED){
		Vec2 m = TransformedView_ScreenWorldPos(&tv,GetMouse());
		if(m.x>=0 && m.x<MAP_WIDTH && m.y>=0 && m.y<MAP_HEIGHT){
			AStarNode* an = nodes + ((int)m.y * MAP_WIDTH + (int)m.x);
			if(Stroke(ALX_KEY_W).DOWN){
				start = an;
			}else if(Stroke(ALX_KEY_S).DOWN){
				end = an;
			}else{
				WorldPoint2D* wp = (WorldPoint2D*)an->data;
				wp->t = !wp->t;
			}	
		}
		else if(Stroke(ALX_KEY_W).DOWN) start = NULL;
		else if(Stroke(ALX_KEY_S).DOWN) end = NULL;

		AStarNode_UpdateNeighbours();
	}
	
	Clear(BLACK);

	for(int i = 0;i<MAP_HEIGHT;i++){
		for(int j = 0;j<MAP_WIDTH;j++){
			AStarNode* an = nodes + (i * MAP_WIDTH + j);
			WorldPoint2D* awp = (WorldPoint2D*)an->data;
			
			for(int k = 0;k<an->neighbours.size;k++){
				AStarNode* nan = *(AStarNode**)Vector_Get(&an->neighbours,k);
				WorldPoint2D* nwp = (WorldPoint2D*)nan->data;

				Vec2 ap = TransformedView_WorldScreenPos(&tv,(Vec2){ awp->x + 0.5f,awp->y + 0.5f });
				Vec2 np = TransformedView_WorldScreenPos(&tv,(Vec2){ nwp->x + 0.5f,nwp->y + 0.5f });
				RenderLine(ap,np,BLUE,1.0f);
			}
		}
	}

	for(int i = 0;i<MAP_HEIGHT;i++){
		for(int j = 0;j<MAP_WIDTH;j++){
			AStarNode* an = nodes + (i * MAP_WIDTH + j);
			WorldPoint2D* wp = (WorldPoint2D*)an->data;
			
			Vec2 p = TransformedView_WorldScreenPos(&tv,(Vec2){ wp->x + 0.05f,wp->y + 0.05f });
			Vec2 d = TransformedView_WorldScreenLength(&tv,(Vec2){ 0.9f,0.9f });
			RenderRect(p.x,p.y,d.x,d.y,an == start ? GREEN : (an == end ? RED : (wp->t ? WHITE : (an->visited ? LIGHT_BLUE : BLUE))));
		}
	}

	if(start && end){
		for(int i = 0;i<MAP_WIDTH * MAP_HEIGHT;i++){
			AStarNode* an = nodes + i;
			an->visited = 0;
			an->local = INFINITY;
			an->global = INFINITY;
			an->parent = NULL;
		}
		AStarNode_UpdateND(start,end,2);
		
		AStarNode* p = end;
		while(p->parent){
			WorldPoint2D* awp = (WorldPoint2D*)p->data;
			WorldPoint2D* pwp = (WorldPoint2D*)p->parent->data;

			Vec2 ap = TransformedView_WorldScreenPos(&tv,(Vec2){ awp->x + 0.5f,awp->y + 0.5f });
			Vec2 pp = TransformedView_WorldScreenPos(&tv,(Vec2){ pwp->x + 0.5f,pwp->y + 0.5f });
			RenderLine(ap,pp,YELLOW,1.0f);

			p = p->parent;
		}
	}
	
	//String str = String_Format("MI:%d",nMaxIterations);
	//CStr_RenderSizeAlxFont(WINDOW_STD_ARGS,&window.AlxFont,str.Memory,str.size,0.0f,0.0f,WHITE);
	//String_Free(&str);
}
void Delete(AlxWindow* w){
	for(int i = 0;i<MAP_WIDTH * MAP_HEIGHT;i++){
		AStarNode_Free(nodes + i);
	}
	
	if(nodes) free(nodes);
	nodes = NULL;
}

int main(){
    if(Create("A* Path Finder",1920,1080,1,1,Setup,Update,Delete))
        Start();
    return 0;
}