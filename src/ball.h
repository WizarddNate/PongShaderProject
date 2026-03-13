#pragma once
#include "cpup/canis.h"
#include "cpup/scene.h"
#include "cpup/model.h"
#include "cpup/inputmanager.h"

#include <SDL3/SDL.h>
#include "notball.h"

typedef struct {
    int Lscore;
    int Rscore;
    int trailLength;
    bool areBallsSpawned;
    int ballCount;
    bool isMainBall;

    void (*SpawnRandBalls)(AppContext*);
} Ball;

Entity* SpawnBall(AppContext* _app, Entity* _entity);
void SpawnRandBalls(AppContext* _app, Entity* _entity);

void BallStart(AppContext* _app, Entity* _entity) {
    _entity->color = InitVector4(1.0f, 1.0f, 1.0f, 1.0f);

    _entity->transform.scale = InitVector3(32.0f, 32.0f, 1.0f);

    Ball* BallStart = (Ball*)_entity->data; 
    BallStart->areBallsSpawned = false;
    BallStart->ballCount = 1;
}

void BallUpdate(AppContext* _app, Entity* _entity) {

    //get entities
    Ball* ball = (Ball*)_entity->data; 
    Transform lPaddle = Find(&_app->scene, "leftPaddle")->transform;
    Transform rPaddle = Find(&_app->scene, "rightPaddle")->transform;

    Vector4 lPaddleColor = Find(&_app->scene, "leftPaddle")->color;
    Vector4 rPaddleColor = Find(&_app->scene, "rightPaddle")->color;
    
    //reset game
    if (GetKeyDown(_app, SDL_SCANCODE_R))
    {
        _entity->velocity = (Vector2){0.0f, 0.0f};
        SDL_SetWindowTitle(_app->window, "Game Reset");
        _entity->transform.position = InitVector3(_app->windowWidth * 0.5f, _app->windowHeight * 0.5f, 0.0f);
        _entity->color = InitVector4(1.0f, 1.0f, 1.0f, 1.0f);
    }

    if (Vec2EqualsZero(_entity->velocity) && GetKeyDown(_app, SDL_SCANCODE_SPACE))
    {
        i32 startingDirection = rand() % 4;
        
        static Vector2 directions[4] = {
            (Vector2){0.72f, 0.72f},
            (Vector2){0.72f, -0.72f},
            (Vector2){-0.72f, 0.72f},
            (Vector2){-0.72f, -0.72f},
        };

        _entity->velocity = Vec2Mul(directions[startingDirection], 150.0f);

        char buffer[100];

        // Format data into the buffer
        sprintf(buffer, "Left Score: %i | Right score: %i", ball->Lscore, ball->Rscore);
        
        SDL_SetWindowTitle(_app->window, buffer);
    }

    // check if ball is heading below the screen
    if (_entity->transform.position.y - _entity->transform.scale.y * 0.5f <= 0.0f && _entity->velocity.y < 0.0f)
        _entity->velocity.y *= -1.0f; 
    
    // check if ball is heading above the screen
    if (_entity->transform.position.y + _entity->transform.scale.y * 0.5f >= _app->windowHeight && _entity->velocity.y > 0.0f)
        _entity->velocity.y *= -1.0f; 

    //check if ball is heading to left of screen
    if (_entity->transform.position.x + _entity->transform.scale.x * 0.5f <= 0.0f){
        _entity->transform.position = InitVector3(_app->windowWidth * 0.5f, _app->windowHeight * 0.5f, 0.0f);
        _entity->velocity = (Vector2){0.0f, 0.0f};
        _entity->color = InitVector4(1.0f, 1.0f, 1.0f, 1.0f);
        ball->Rscore++;
        SDL_SetWindowTitle(_app->window, "+1 for Right");
    }
    
    //check if ball is heading to right of screen
    if (_entity->transform.position.x - _entity->transform.scale.x * 0.5f >= _app->windowWidth){
        _entity->transform.position = InitVector3(_app->windowWidth * 0.5f, _app->windowHeight * 0.5f, 0.0f);
        _entity->velocity = (Vector2){0.0f, 0.0f};
        _entity->color = InitVector4(1.0f, 1.0f, 1.0f, 1.0f);
        ball->Lscore++;
        SDL_SetWindowTitle(_app->window, "+1 for Left");
    }

    //Win game at 5
    if (ball->Lscore == 5|| ball-> Rscore == 5){
        //reset ball
        _entity->transform.position = InitVector3(_app->windowWidth * 0.5f, _app->windowHeight * 0.5f, 0.0f);
        _entity->velocity = (Vector2){0.0f, 0.0f};
        
        //spawn several balls
        if (ball->areBallsSpawned == false){
            SpawnRandBalls(_app, _entity);
            ball->areBallsSpawned = true;
        }

        //call out winner
        if (ball->Lscore >= 5){
            SDL_SetWindowTitle(_app->window, "Left Wins!");
        }
        else{
            SDL_SetWindowTitle(_app->window, "Right Wins!");
        }
    }

    //Paddle collision
    
    //ball position and scale set as variables so if statement is more readable
    Vector2 ball_pos;
    ball_pos.x = _entity->transform.position.x;
    ball_pos.y = _entity->transform.position.y; 

    Vector2 ball_scale;
    ball_scale.x = _entity->transform.scale.x;
    ball_scale.y = _entity->transform.scale.y;

    if (ball_pos.x - (ball_scale.x  * 0.5) < lPaddle.position.x + lPaddle.scale.x * 0.5 &&
        ball_pos.y - (ball_scale.y * 0.5) < lPaddle.position.y + lPaddle.scale.y *0.5 &&
        ball_pos.y + (ball_scale.y * 0.5) > lPaddle.position.y - lPaddle.scale.y *0.5){
            _entity->velocity.x *= -1.1f;
            _entity->color = lPaddleColor;
    }
    if (ball_pos.x + (ball_scale.x  * 0.5) > rPaddle.position.x - (rPaddle.scale.x * 0.5) &&
        ball_pos.y - (ball_scale.y * 0.5) < rPaddle.position.y +  rPaddle.scale.y *0.5 &&
        ball_pos.y + (ball_scale.y * 0.5) > rPaddle.position.y - rPaddle.scale.y *0.5){
            _entity->velocity.x *= -1.1f;
            _entity->color = rPaddleColor;
    }

    //ball movement
    Vector3 delta = Vec2ToVec3(Vec2Mul(_entity->velocity, _app->deltaTime));
    _entity->transform.position = Vec3Add(_entity->transform.position, delta);
}

//check number of balls
void SpawnRandBalls(AppContext* _app, Entity* _entity){

    printf("Spawnning win balls \n");
    for(int i = 0; i < 200; i++){
        int random_x = rand() % _app->windowWidth;
        int random_y = rand() % _app->windowHeight;
        SpawnNotBall(_app, _entity);
    }
    //duplicate ball script
    //include in ball 
    //include its own spawn ball effect
} 

void BallDraw(AppContext* _app, Entity* _entity) {
    Matrix4 transform = IdentityMatrix4(); // the order is important
    Mat4Translate(&transform, _entity->transform.position);
    Mat4Rotate(&transform, _entity->transform.rotation * DEG2RAD, InitVector3(0.0f, 0.0f, 1.0f));
    Mat4Scale(&transform, InitVector3(_entity->transform.scale.x, _entity->transform.scale.y, _entity->transform.scale.z));

    BindShader(_entity->shaderId);

    ShaderSetFloat(_entity->shaderId, "TIME", _app->time);
    ShaderSetMatrix4(_entity->shaderId, "VIEW", _app->view);
    ShaderSetMatrix4(_entity->shaderId, "PROJECTION", _app->projection);

    ShaderSetVector4(_entity->shaderId, "COLOR", _entity->color);
    ShaderBindTexture(_entity->shaderId, _entity->image->id, "MAIN_TEXTURE", 0);
    ShaderSetMatrix4(_entity->shaderId, "TRANSFORM", transform);
    DrawModel(*_entity->model);

    UnBindShader();
}

void BallOnDestroy(AppContext* _app, Entity* _entity) {

}

Entity* SpawnBall(AppContext* _app, Entity* _entity) {
    Entity* ball = Spawn(&(_app->scene));
    ball->transform.position = InitVector3(_app->windowWidth * 0.5f, _app->windowHeight * 0.5f, 0.0f);
    ball->data = calloc(1, sizeof(Ball));
    ball->image = _entity->image;
    ball->model = _entity->model;
    ball->shaderId = _entity->shaderId;
    ball->Start = BallStart;
    ball->Update = BallUpdate;
    ball->Draw = BallDraw;
    ball->OnDestroy = BallOnDestroy;
    return ball;
}