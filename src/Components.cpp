#pragma once

#include <unordered_map>
#include <queue>
#include "SDL2/SDL.h"

namespace Components {

SDL_Texture *arrow_png;
SDL_Renderer *renderer;


class Component {
    public:
        SDL_Rect pos;
        virtual void Render() = 0;
        virtual void Execute() = 0;
        bool IsPointInside(int x ,int y){
            SDL_Point point = {.x = x , .y = y};
            return SDL_PointInRect(&point, &pos);
        }
};

class NextBtn : public Component {
    private:
        SDL_Rect *parent;

    public:
        NextBtn(int relevant_x, int relevant_y ,SDL_Rect *parent){
            this->parent = parent;

            this->pos.x = relevant_x + parent->x     ;
            this->pos.y = relevant_y + parent->y     ;
            this->pos.h = parent->h                  ;
            this->pos.w = this->pos.h*(194.0f/322.0f);

        }
        void Render() override{
            SDL_RenderCopyEx(renderer, arrow_png, NULL, &pos, 0.0, NULL, SDL_FLIP_NONE);
        }
        void Execute() override{
            //tracker->Next();
        }

};

class PrevBtn : public Component {
    private:
        SDL_Rect *parent;

    public:
        PrevBtn(int relevant_x, int relevant_y ,SDL_Rect *parent){
            this->parent = parent;

            this->pos.x = relevant_x + parent->x     ;
            this->pos.y = relevant_y + parent->y     ;
            this->pos.h = parent->h                  ;
            this->pos.w = this->pos.h*(194.0f/322.0f);

        }
        void Render() override{
            SDL_RenderCopyEx(renderer, arrow_png, NULL, &pos, 0.0, NULL, SDL_FLIP_HORIZONTAL);
        }
        void Execute() override{
            //tracker->Prev();
        }

};

std::queue<Component*> components;

} // Components