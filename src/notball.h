#pragma once
#include "cpup/canis.h"
#include "cpup/scene.h"
#include "cpup/model.h"
#include "cpup/inputmanager.h"

#include <SDL3/SDL.h>

typedef struct {
    int Lscore;
    int Rscore;

} NotBall;

Entity* SpawnNotBall(AppContext* _app, Entity* _entity);

void NotBallStart(AppContext* _app, Entity* _entity) {
    _entity->color = InitVector4(1.0f, 1.0f, 0.0f, 1.0f);
    _entity->transform.scale = InitVector3(32.0f, 32.0f, 1.0f);

}

void NotBallUpdate(AppContext* _app, Entity* _entity) {

    Vector2 direction = (Vector2){-0.0f, -1.0f};
    _entity->velocity = Vec2Mul(direction, 250.0f);
    
    Vector3 delta = Vec2ToVec3(Vec2Mul(_entity->velocity, _app->deltaTime));
    _entity->transform.position = Vec3Add(_entity->transform.position, delta);

    // check if ball is heading below the screen
    if (_entity->transform.position.y - _entity->transform.scale.y * 0.5f <= 0.0f && _entity->velocity.y < 0.0f)
        Destroy(_app, &(_app->scene), _entity->id);
}

void NotBallDraw(AppContext* _app, Entity* _entity) {
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

void NotBallOnDestroy(AppContext* _app, Entity* _entity) {

}

Entity* SpawnNotBall(AppContext* _app, Entity* _entity) {
    Entity* notball = Spawn(&(_app->scene));

    int random_x = rand() % _app->windowWidth;
    int random_y = rand() % _app->windowHeight + 0.6f;
    notball->transform.position = InitVector3(random_x, random_y, 0.0f);
    notball->data = calloc(1, sizeof(NotBall));
    notball->image = _entity->image;
    notball->model = _entity->model;
    notball->shaderId = _entity->shaderId;
    notball->Start = NotBallStart;
    notball->Update = NotBallUpdate;
    notball->Draw = NotBallDraw;
    notball->OnDestroy = NotBallOnDestroy;

    return notball;
}