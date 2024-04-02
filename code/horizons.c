#include "horizons.h"
#include "horizons_string.h"

global game_state *State;
global platform_api *Platform;

#include "horizons_json.c"
#include "horizons_gltf.c"

// TODO(evan): Put the light information in state
internal void
DrawMesh(vec3 Position, vec3 Rotation, vec3 Scale, f32 AmbientStrength, vec3 LightDirection, vec3 LightColor, mat4 PrevM, game_material *Material)
{
  mat4 Transform = Mat4CreateTransform3D(Position, Rotation, Scale);
  mat4 M = Mat4Mul(Transform, PrevM);
  
  mat4 Normal4 = Mat4Inverse(&Transform);
  Normal4 = Mat4Transpose(&Normal4);
  mat3 Normal = Mat3FromMat4(&Normal4);
  
  vs_shader_constants VSConstants;
  Platform->CopyMemory(VSConstants.Matrix.Elements, M.Elements, sizeof(mat4));
  Platform->CopyMemory(VSConstants.Transform.Elements, Transform.Elements, sizeof(mat4));
  Platform->CopyMemory(VSConstants.Color.Elements, Material->Color.Elements, sizeof(vec4));
  Platform->CopyMemory(VSConstants.Normal.Elements, Normal.Elements, sizeof(mat3));
  
  ps_shader_constants PSConstants;
  PSConstants.AmbientStrength = AmbientStrength;
  Platform->CopyMemory(PSConstants.LightDirection.Elements, LightDirection.Elements, sizeof(vec3));
  Platform->CopyMemory(PSConstants.LightColor.Elements, LightColor.Elements, sizeof(vec3));
  Platform->CopyMemory(PSConstants.CameraPosition.Elements, State->CameraPosition.Elements, sizeof(vec3));
  
  Platform->DrawMesh(&VSConstants, &PSConstants);
}

GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
  State = (game_state *)Memory->PermanentStorage;
  Platform = &Memory->Platform;
  if(!State->Initialized)
  {
    InitializeArena(&State->PermArena, (u8 *)Memory->PermanentStorage + sizeof(game_state),
                    Memory->PermanentStorageSize - sizeof(game_state));
    InitializeArena(&State->TempArena, (u8 *)Memory->TempStorage, Memory->TempStorageSize);
    
    State->Time = 0;
    
    State->CameraPosition = Vec3(0, 0, 5);
    State->CameraRotation = Vec3(0, 90, 0);
    State->CameraFront = Vec3(0, 0, 0);
    State->CameraUp = Vec3(0, 1, 0);
    
    State->ControllingCharacter = false;
    
    State->CubeRot = 0;
    
    LoadGLTF("cube.gltf", &State->CubeMeshes);
    LoadGLTF("cone.gltf", &State->ConeMeshes);
    
    State->TempArena.Used = 0;
    State->Initialized = true;
  }
  
  State->Time += DeltaTime;
  
  if(Input->Keyboard.E && !State->LastInput.Keyboard.E)
  {
    State->ControllingCharacter = !State->ControllingCharacter;
  }
  
  State->CameraFront = Vec3FPEulerToRotation(State->CameraRotation.x*DEG_TO_RAD,
                                             State->CameraRotation.y*DEG_TO_RAD);
  
  if(State->ControllingCharacter)
  {
    f32 Sensitivity = 25;
    f32 DeltaX = (f32)Input->Mouse.X - State->LastInput.Mouse.X;
    f32 DeltaY = (f32)Input->Mouse.Y - State->LastInput.Mouse.Y;
    f32 MouseX = DeltaX*Sensitivity*DeltaTime;
    f32 MouseY = DeltaY*Sensitivity*DeltaTime;
    
    State->CameraRotation.Pitch += MouseY;
    if(State->CameraRotation.Pitch > 89)
    {
      State->CameraRotation.Pitch = 89;
    }
    if(State->CameraRotation.Pitch < -89)
    {
      State->CameraRotation.Pitch = -89;
    }
    
    // TODO(evan): Make sure this is between 0 and 360
    State->CameraRotation.Yaw -= MouseX;
    
    
    f32 Speed = 12;
    vec2 Movement = {0};
    if(Input->Keyboard.W)
    {
      Movement.z -= 1;
    }
    if(Input->Keyboard.S)
    {
      Movement.z += 1;
    }
    if(Input->Keyboard.D)
    {
      Movement.x += 1;
    }
    if(Input->Keyboard.A)
    {
      Movement.x -= 1;
    }
    Movement = Vec2Normalize(Movement);
    Movement = Vec2Scale(Movement, Speed*DeltaTime);
    
    vec3 CameraGroundedRotation =
      Vec3Normalize(Vec3(State->CameraFront.x, 0, State->CameraFront.z));
    State->CameraPosition =
      Vec3Add(State->CameraPosition, Vec3Scale(CameraGroundedRotation, Movement.z));
    State->CameraPosition =
      Vec3Add(State->CameraPosition,
              Vec3Scale(Vec3Normalize(Vec3Cross(CameraGroundedRotation,
                                                State->CameraUp)),
                        Movement.x));
  }
  
  f32 Aspect = (f32)WindowDimension.Width / WindowDimension.Height;
  mat4 Perspective = Mat4Perspective(Aspect, 60, 0.01f, 100);
  mat4 View = Mat4View(State->CameraPosition, State->CameraFront, State->CameraUp);
  mat4 PrevM = Mat4Mul(View, Perspective);
  
  f32 AmbientStrength = .1f;
  vec3 LightDirection = Vec3(-.2f, -1, -.3f);
  vec3 LightColor = Vec3(.3f, .3f, .3f);
  
  State->CubeRot += 30*DeltaTime;
  Platform->SetMesh(State->CubeMeshes[0].Mesh);
  DrawMesh(Vec3(0, 0, 0), Vec3(State->CubeRot + 45, State->CubeRot, State->CubeRot + 135), Vec3(1, 1, 1), AmbientStrength, LightDirection, LightColor, PrevM, State->CubeMeshes[0].Material);
  
  Platform->SetMesh(State->ConeMeshes[0].Mesh);
  DrawMesh(Vec3(5, 0, 0), Vec3(State->CubeRot + 45, State->CubeRot, State->CubeRot + 135), Vec3(1, 1, 1), AmbientStrength, LightDirection, LightColor, PrevM, State->ConeMeshes[0].Material);
  
  State->LastInput = *Input;
  State->TempArena.Used = 0;
  return(Input->Keyboard.Escape);
}