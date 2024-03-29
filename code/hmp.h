/* date = February 18th 2024 4:31 pm */

#ifndef HMP_H
#define HMP_H

#pragma pack(push, 1)
typedef struct hmp_header
{
  u32 Width;
  u32 Height;
} hmp_header;
#pragma pack(pop)

typedef struct hmp_data
{
  u32 Width;
  u32 Height;
  u32 *Pixels;
} hmp_data;

internal game_sprite
LoadHMP(s8 *FileName, platform_api *Platform,
        memory_arena *Arena)
{
  string8 FileNameStr = CreateString(FileName, Arena, Platform);
  platform_file_handle File = Platform->OpenFile(FileNameStr, true, FILE_OPEN_READ);
  u32 FileSize = Platform->GetFileSize(File);
  u8 *Data = PushSize(Arena, FileSize);
  Platform->ReadEntireFile(File, FileSize, Data);
  Platform->CloseFile(File);
  
  hmp_header *Header = (hmp_header *)Data;
  u32 *Pixels = (u32 *)(Data + sizeof(hmp_header));
  
  game_sprite Result;
  Result.Size = (vec2){(f32)Header->Width, (f32)Header->Height};
  Result.HalfSize = Vec2DivideScalar(Result.Size, 2);
  Result.Sprite = Platform->CreateSprite(Pixels, Header->Width, Header->Height);
  
  return(Result);
}

#endif //HMP_H
