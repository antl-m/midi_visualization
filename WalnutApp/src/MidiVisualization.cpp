#include "MidiVisualization.h"
#include "windows.h"
#include "imgui.h"

#include <optional>
#include <filesystem>

//
// Service
//

namespace
{

ImVec2 operator+(ImVec2 lhs, ImVec2 rhs)
{
  return ImVec2{ lhs.x + rhs.x, lhs.y + rhs.y };
}

ImVec2 operator-(ImVec2 lhs, ImVec2 rhs)
{
  return ImVec2{ lhs.x - rhs.x, lhs.y - rhs.y };
}

float operator*(ImVec2 lhs, ImVec2 rhs)
{
  return lhs.x * rhs.x + lhs.y * rhs.y;
}

ImVec2 operator*(ImVec2 lhs, float rhs)
{
  return ImVec2{ lhs.x * rhs, lhs.y * rhs };
}

ImVec2 operator*(float lhs, ImVec2 rhs)
{
  return ImVec2{ lhs * rhs.x, lhs * rhs.y };
}

ImVec2 operator/(ImVec2 lhs, float rhs)
{
  return ImVec2{ lhs.x / rhs, lhs.y / rhs };
}

float Lerp(float a, float b, float t)
{
  return a + t * (b - a);
}

ImVec2 CalculateEllipsePoint(ImVec2 _EllipseSize, float _T)
{
  return { _EllipseSize.x + _EllipseSize.x * std::cos(_T), _EllipseSize.y + _EllipseSize.y * std::sin(_T) };
}

float ModuloAdd(float a, float b)
{
  float ab = a + b;
  return ab - std::floorf(ab);
}

void Spinner(float _Size, std::size_t N, float _Speed)
{
  static float T = 0.0f;

  static constexpr float PI               = 3.141592f;
  static constexpr float DOUBLE_PI        = 2 * PI;
  static constexpr float MIN_CIRCLE_SCALE = 0.1f;

  const float DeltaT = DOUBLE_PI / N;
  const float SinPiN = std::sin(PI / N);

  const float BigRadius   = _Size / (2 * SinPiN + 2);
  const float SmallRadius = BigRadius * SinPiN;

  ImGui::BeginChild("Spinner", ImVec2(_Size, _Size));

  const ImVec2 CursorPos = ImGui::GetCursorScreenPos() + ImVec2(SmallRadius, SmallRadius);

  for (std::size_t i = 0; i < N; ++i)
  {
    float x = 1 - static_cast<float>(i) / N;

    x = ModuloAdd(x, T);

    ImGui::GetWindowDrawList()->AddCircleFilled(
        CursorPos + CalculateEllipsePoint(ImVec2(BigRadius, BigRadius), i * DeltaT),
        SmallRadius * (std::pow(2 * x - 1, 2.0f) / (1 / (1 - MIN_CIRCLE_SCALE)) + MIN_CIRCLE_SCALE),
        0xFFFFFFFF
      );
  }

  ImGui::EndChild();

  T = ModuloAdd(T, ImGui::GetIO().DeltaTime / _Speed);
}

float GetFitSize()
{
  const auto Size = ImGui::GetContentRegionAvail();
  return min(Size.x, Size.y);
}

float EaseInCubic(float x)
{
  return x * x * x;
}

} // namespace

//
// Animation
//

ImU32 MixColor(
    ImU32 _Color,
    float _Opacity
  )
{
  return (_Color & 0x00ffffff) | (static_cast<ImU32>(0xff * _Opacity) << 24);
}

void DrawCircle(
    ImVec2 _ScreenPos,
    float  _Height,
    bool   _Filled,
    float  _Opacity,
    ImU32  _Color
  )
{
  if (_Filled)
    ImGui::GetWindowDrawList()->AddCircleFilled(_ScreenPos, _Height / 2, MixColor(_Color, _Opacity));
  else
    ImGui::GetWindowDrawList()->AddCircle(_ScreenPos, _Height / 2, MixColor(_Color, _Opacity));
}

void DrawTriangle(
    ImVec2 _ScreenPos,
    float  _Height,
    bool   _Filled,
    float  _Opacity,
    ImU32  _Color
  )
{
  ImGui::GetWindowDrawList()->PathClear();

  //                                 sqrt(3)
  const float Side = (2 * _Height) / 1.732050807568877;
  const float Half = _Height / 2;

  ImGui::GetWindowDrawList()->PathLineTo(_ScreenPos + ImVec2(        0, -Half));
  ImGui::GetWindowDrawList()->PathLineTo(_ScreenPos + ImVec2( Side / 2,  Half));
  ImGui::GetWindowDrawList()->PathLineTo(_ScreenPos + ImVec2(-Side / 2,  Half));
  ImGui::GetWindowDrawList()->PathLineTo(_ScreenPos + ImVec2(        0, -Half));

  if (_Filled)
    ImGui::GetWindowDrawList()->PathFillConvex(MixColor(_Color, _Opacity));
  else
    ImGui::GetWindowDrawList()->PathStroke(MixColor(_Color, _Opacity));
}

void DrawTriangleUpsideDown(
    ImVec2 _ScreenPos,
    float  _Height,
    bool   _Filled,
    float  _Opacity,
    ImU32  _Color
  )
{
  ImGui::GetWindowDrawList()->PathClear();

  //                                 sqrt(3)
  const float Side = (2 * _Height) / 1.732050807568877;
  const float Half = _Height / 2;

  ImGui::GetWindowDrawList()->PathLineTo(_ScreenPos + ImVec2(        0,  Half));
  ImGui::GetWindowDrawList()->PathLineTo(_ScreenPos + ImVec2( Side / 2, -Half));
  ImGui::GetWindowDrawList()->PathLineTo(_ScreenPos + ImVec2(-Side / 2, -Half));
  ImGui::GetWindowDrawList()->PathLineTo(_ScreenPos + ImVec2(        0,  Half));

  if (_Filled)
    ImGui::GetWindowDrawList()->PathFillConvex(MixColor(_Color, _Opacity));
  else
    ImGui::GetWindowDrawList()->PathStroke(MixColor(_Color, _Opacity));
}

void DrawSquare(
    ImVec2 _ScreenPos,
    float  _Height,
    bool   _Filled,
    float  _Opacity,
    ImU32  _Color
  )
{
  const auto Offset = ImVec2(_Height / 2, _Height / 2);

  if (_Filled)
    ImGui::GetWindowDrawList()->AddRectFilled(_ScreenPos - Offset, _ScreenPos + Offset, MixColor(_Color, _Opacity));
  else
    ImGui::GetWindowDrawList()->AddRect(_ScreenPos - Offset, _ScreenPos + Offset, MixColor(_Color, _Opacity));
}

void DrawRhombus(
    ImVec2 _ScreenPos,
    float  _Height,
    bool   _Filled,
    float  _Opacity,
    ImU32  _Color
  )
{
  ImGui::GetWindowDrawList()->PathClear();

  const float Half = _Height / 2;

  ImGui::GetWindowDrawList()->PathLineTo(_ScreenPos + ImVec2(    0, -Half));
  ImGui::GetWindowDrawList()->PathLineTo(_ScreenPos + ImVec2( Half,     0));
  ImGui::GetWindowDrawList()->PathLineTo(_ScreenPos + ImVec2(    0,  Half));
  ImGui::GetWindowDrawList()->PathLineTo(_ScreenPos + ImVec2(-Half,     0));
  ImGui::GetWindowDrawList()->PathLineTo(_ScreenPos + ImVec2(    0, -Half));

  if (_Filled)
    ImGui::GetWindowDrawList()->PathFillConvex(MixColor(_Color, _Opacity));
  else
    ImGui::GetWindowDrawList()->PathStroke(MixColor(_Color, _Opacity));
}

void DrawPentagon(
    ImVec2 _ScreenPos,
    float  h,
    bool   _Filled,
    float  _Opacity,
    ImU32  _Color
  )
{
  ImGui::GetWindowDrawList()->PathClear();

  const float a = h * 0.6498393924658126;
  const float d = a * 1.618033988749895;
  const float y = std::sqrt(h * h - d * d) / 2;

  ImGui::GetWindowDrawList()->PathLineTo(_ScreenPos + ImVec2(     0, -h / 2));
  ImGui::GetWindowDrawList()->PathLineTo(_ScreenPos + ImVec2( d / 2,     -y));
  ImGui::GetWindowDrawList()->PathLineTo(_ScreenPos + ImVec2( a / 2,  h / 2));
  ImGui::GetWindowDrawList()->PathLineTo(_ScreenPos + ImVec2(-a / 2,  h / 2));
  ImGui::GetWindowDrawList()->PathLineTo(_ScreenPos + ImVec2(-d / 2,     -y));
  ImGui::GetWindowDrawList()->PathLineTo(_ScreenPos + ImVec2(     0, -h / 2));

  if (_Filled)
    ImGui::GetWindowDrawList()->PathFillConvex(MixColor(_Color, _Opacity));
  else
    ImGui::GetWindowDrawList()->PathStroke(MixColor(_Color, _Opacity));
}

void DrawHexagon(
    ImVec2 _ScreenPos,
    float  h,
    bool   _Filled,
    float  _Opacity,
    ImU32  _Color
  )
{
  ImGui::GetWindowDrawList()->PathClear();

  const float a   = h * 0.5773502691896258;
  const float a_2 = a / 2;
  const float h_2 = h / 2;

  ImGui::GetWindowDrawList()->PathLineTo(_ScreenPos + ImVec2( a_2, -h_2));
  ImGui::GetWindowDrawList()->PathLineTo(_ScreenPos + ImVec2(   a,    0));
  ImGui::GetWindowDrawList()->PathLineTo(_ScreenPos + ImVec2( a_2,  h_2));
  ImGui::GetWindowDrawList()->PathLineTo(_ScreenPos + ImVec2(-a_2,  h_2));
  ImGui::GetWindowDrawList()->PathLineTo(_ScreenPos + ImVec2(  -a,    0));
  ImGui::GetWindowDrawList()->PathLineTo(_ScreenPos + ImVec2(-a_2, -h_2));
  ImGui::GetWindowDrawList()->PathLineTo(_ScreenPos + ImVec2( a_2, -h_2));

  if (_Filled)
    ImGui::GetWindowDrawList()->PathFillConvex(MixColor(_Color, _Opacity));
  else
    ImGui::GetWindowDrawList()->PathStroke(MixColor(_Color, _Opacity));
}

struct DrawTrackSetup
{
  void(*DrawFunction)(ImVec2, float, bool, float, ImU32);
  ImU32 Color;
};

const std::vector<DrawTrackSetup> TRACK_SETUPS {
    { &DrawCircle,             0x12B0FF },
    { &DrawTriangle,           0xA3C54D },
    { &DrawSquare,             0xA3DAEE },
    { &DrawRhombus,            0x9CACE9 },
    { &DrawPentagon,           0x3F82FE },
    { &DrawHexagon,            0x92780F },
    { &DrawTriangleUpsideDown, 0x729CB5 },
    { &DrawCircle,             0xA88F7A },
    { &DrawTriangle,           0x3854A2 },
    { &DrawSquare,             0x8C5633 },
    { &DrawRhombus,            0x465D57 },
    { &DrawPentagon,           0x7B6171 },
    { &DrawHexagon,            0x3F39D1 },
    { &DrawTriangleUpsideDown, 0xF2F0EE },
    { &DrawCircle,             0x9B9CA0 },
    { &DrawTriangle,           0x4C3D3B },
  };

//
// Constants
//

const std::string MidiVisualization::FILES_DIR = "rsc";
const std::string MidiVisualization::TEMP_FILE = "TempFile.wav";

//
// Walnut::Layer
//

void MidiVisualization::OnAttach()
{
  RescanDirectory();
}

void MidiVisualization::OnUIRender()
{
  const bool WaitProcess = IsFileProcessing();

  ImGui::Begin("Midi");
  RenderFileControls();

  ImGui::Separator();

  if (WaitProcess)
    Spinner(GetFitSize(), 10, 1);

  if (m_IsProcessed)
    RenderMidiContent();
  ImGui::End();


  ImGui::Begin("Animation");
  RenderFileControls();

  ImGui::Separator();

  if (WaitProcess)
    Spinner(GetFitSize(), 10, 1);

  if (m_IsProcessed)
    RenderAnimation();
  ImGui::End();
}

void MidiVisualization::OnUpdate(
    float _DeltaTime
  )
{
  if (m_IsPlaying && m_Time < m_MidiFile.getFileDurationInSeconds())
    m_Time += _DeltaTime;
}

//
// Service UI
//

void MidiVisualization::RenderFileControls()
{
  const bool IsOpened = ImGui::TreeNode("File controls");

  if (!IsOpened)
    return;

  if (ImGui::Button("Rescan directory"))
    RescanDirectory();

  if (!m_FileName.empty() && !m_IsProcessed && !IsFileProcessing())
  {
    ImGui::SameLine();

    if (ImGui::Button("Process file"))
      StartProcessFile(m_FileName);
  }

  if (m_IsProcessed)
  {
    ImGui::SameLine();

    if (!m_IsPlaying && ImGui::Button("Play"))
      StartPlaying();
    else
    if (m_IsPlaying && ImGui::Button("Stop"))
      StopPlaying();
  }

  ImGui::BeginDisabled(IsFileProcessing());
  if (ImGui::BeginCombo("##Combo", m_FileName.c_str()))
  {
    for (const auto & Entry : m_DirectoryFiles)
    {
      const bool IsSelected = (Entry == m_FileName);
      if (ImGui::Selectable(Entry.c_str(), IsSelected))
      {
        m_FileName = Entry;
        m_IsProcessed = false;
        StopPlaying();
      }
      if (IsSelected)
        ImGui::SetItemDefaultFocus();
    }
    ImGui::EndCombo();
  }
  ImGui::EndDisabled();

  if (IsOpened)
    ImGui::TreePop();
}

void MidiVisualization::RenderMidiContent()
{
  if (ImGui::TreeNode("Display settings"))
  {
    ImGui::SliderFloat("Pixels per second", &m_PixelPerSecond, 10, 1000);
    ImGui::SliderFloat("Note height",       &m_NoteHeight,      3,   20);
    ImGui::Checkbox("Follow", &m_Follow);

    ImGui::TreePop();
  }

  ImGui::Separator();

  if (m_Follow)
    m_TrackOffset = m_Time - ImGui::GetContentRegionAvail().x / (2 * m_PixelPerSecond);

  const auto DragDelta   = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
  const auto TrackOffset = m_Follow ? m_TrackOffset : m_TrackOffset - DragDelta.x / m_PixelPerSecond;

  ImGui::BeginChild("##Tracks", ImVec2(-1, -1));

  if (!m_Follow &&
      ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows) &&
      ImGui::IsMouseReleased(ImGuiMouseButton_Left))
  {
    m_TrackOffset -= DragDelta.x / m_PixelPerSecond;
  }

  for (int TrackIdx = 0; TrackIdx < m_MidiFile.getTrackCount(); ++TrackIdx)
  {
    const auto & Track      = m_MidiFile[TrackIdx];
    const auto   EventCount = Track.getEventCount();
    const auto   MinNote    = m_TrackNoteRange[TrackIdx].first;
    const auto   MaxNote    = m_TrackNoteRange[TrackIdx].second;
    const auto   NoteRange  = MaxNote - MinNote + 1;
    const auto   Message    = m_TrackMetaMessage[TrackIdx];

    ImGui::PushID(TrackIdx);

    ImGui::NewLine();
    ImGui::Separator();

    ImGui::Text("Track No %d", TrackIdx);

    if (!Message.empty() && ImGui::TreeNode("Meta message"))
    {
      ImGui::TextUnformatted(Message.c_str());
      ImGui::TreePop();
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::BeginChild("##Track", ImVec2(-1, NoteRange * m_NoteHeight), true);

    const auto P = ImGui::GetCursorScreenPos();

    for (int EventIdx = 0; EventIdx < EventCount; ++EventIdx)
    {
      const auto & Event = Track[EventIdx];

      if (!Event.isNoteOn())
        continue;

      const auto BeginPos = ImVec2(
          P.x + (Event.seconds - TrackOffset) * m_PixelPerSecond,
          P.y + (MaxNote - Event.getKeyNumber()) * m_NoteHeight
        );

      const auto EndPos = ImVec2(
          BeginPos.x + Event.getDurationInSeconds() * m_PixelPerSecond - 1,
          BeginPos.y + m_NoteHeight
        );

      ImGui::GetWindowDrawList()->AddRectFilled(
        BeginPos,
        EndPos,
        0xffffffff
      );
    }

    ImGui::GetWindowDrawList()->AddLine(
        ImVec2(P.x + (m_Time - TrackOffset) * m_PixelPerSecond, P.y),
        ImVec2(P.x + (m_Time - TrackOffset) * m_PixelPerSecond, P.y + NoteRange * m_NoteHeight),
        0xff1111ff,
        2
      );

    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopID();
  }

  ImGui::EndChild();
}

void MidiVisualization::RenderAnimation()
{
  if (ImGui::TreeNode("Display controls"))
  {
    ImGui::SliderFloat("Pixels per sec", &m_Anim.PixelPerSec, 10, 1000);
    ImGui::SliderFloat("Figure height", &m_Anim.FigureHeight, 10, 1000);
    ImGui::Checkbox("Render grid", &m_Anim.RenderGrid);

    ImGui::TreePop();
  }

  ImGui::Separator();

  const auto AvailSize      = ImGui::GetContentRegionAvail();
  const auto HalfScreenTime = AvailSize.x / (2 * m_Anim.PixelPerSec);
  const auto TrackOffset    = m_Time - HalfScreenTime;
  const auto NoteRange      = m_Anim.MaxNote - m_Anim.MinNote + 1;
  const auto NoteHeight     = AvailSize.y / (NoteRange + 1);

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
  ImGui::BeginChild("##Animation", ImVec2(-1, -1));

  const auto P = ImGui::GetCursorScreenPos();

  int DrawSetupIdx = 0;

  for (int TrackIdx = 0; TrackIdx < m_MidiFile.getTrackCount(); ++TrackIdx)
  {
    const auto & Track      = m_MidiFile[TrackIdx];
    const auto   EventCount = Track.getEventCount();

    if (!m_TrackHasNote[TrackIdx])
      continue;

    const auto & DrawSetup = TRACK_SETUPS.at(DrawSetupIdx++ % TRACK_SETUPS.size());

    for (int EventIdx = 0; EventIdx < EventCount; ++EventIdx)
    {
      const auto & Event = Track[EventIdx];

      if (!Event.isNoteOn())
        continue;

      auto ScreenPos = P + ImVec2(
          (Event.seconds - TrackOffset) * m_Anim.PixelPerSec,
          (m_Anim.MaxNote - Event.getKeyNumber() + 1) * NoteHeight
        );

      if (Event.seconds < TrackOffset || Event.seconds > m_Time + HalfScreenTime)
        continue;

      if (Event.seconds > m_Time)
      {
        const auto AppearingProgress = EaseInCubic(1 - (Event.seconds - m_Time) / HalfScreenTime);

        ScreenPos.x = Lerp(P.x + AvailSize.x, P.x + AvailSize.x / 2, AppearingProgress);

        DrawSetup.DrawFunction(
            ScreenPos,
            m_Anim.FigureHeight * (Event.getVelocity() / 127.0f) * AppearingProgress,
            false,
            AppearingProgress,
            DrawSetup.Color
          );
      }
      else
      {
        const float DisappearingProgress = (m_Time - Event.seconds) / Event.getDurationInSeconds();

        if (DisappearingProgress < 1)
          DrawSetup.DrawFunction(
            ScreenPos,
            m_Anim.FigureHeight * (Event.getVelocity() / 127.0f) * (1 - EaseInCubic(DisappearingProgress)),
            true,
            1 - DisappearingProgress,
            DrawSetup.Color
          );
      }
    }
  }

  if (m_Anim.RenderGrid)
  {
    ImGui::GetWindowDrawList()->AddLine(
      P + ImVec2((m_Time - TrackOffset) * m_Anim.PixelPerSec, 0),
      P + ImVec2((m_Time - TrackOffset) * m_Anim.PixelPerSec, AvailSize.y),
      0xff1111ff
    );

    for (auto i = m_Anim.MinNote; i <= m_Anim.MaxNote; ++i)
    {
      const auto y = (m_Anim.MaxNote - i + 1) * NoteHeight;

      ImGui::GetWindowDrawList()->AddLine(
        P + ImVec2(0, y), P + ImVec2(AvailSize.x, y),
        0xff1111ff
      );
    }
  }

  ImGui::EndChild();
  ImGui::PopStyleVar();
}

//
// Service
//

void MidiVisualization::StartProcessFile(
    const std::string & _FileName
  )
{
  if (m_IsPlaying)
    StopPlaying();

  m_ProcessFileFuture = std::async(std::launch::async, &MidiVisualization::ProcessFile, this, _FileName);
}

void MidiVisualization::ProcessFile(
    const std::string & _FileName
  )
{
  std::string Parameters = FILES_DIR + "\\" + _FileName + " -Ow -o " + FILES_DIR + "\\" + TEMP_FILE;

  SHELLEXECUTEINFOA ShExecInfo = { 0 };
  ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
  ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
  ShExecInfo.hwnd = NULL;
  ShExecInfo.lpVerb = NULL;
  ShExecInfo.lpFile = "C:\\Program Files\\TiMidity++-2.15.0\\timidity.exe";
  ShExecInfo.lpParameters = Parameters.c_str();
  ShExecInfo.lpDirectory = NULL;
  ShExecInfo.nShow = SW_HIDE;
  ShExecInfo.hInstApp = NULL;
  ShellExecuteExA(&ShExecInfo);

  m_MidiFile.read(FILES_DIR + "\\" + _FileName);

  m_MidiFile.doTimeAnalysis();
  m_MidiFile.linkNotePairs();

  const auto TrackCount = m_MidiFile.getTrackCount();

  m_TrackNoteRange.assign(TrackCount, { 0.0f, 0.0f });
  m_TrackMetaMessage.assign(TrackCount, "");
  m_TrackHasNote.assign(TrackCount, false);
  m_FirstNoteTime = FLT_MAX;

  std::optional<float> GlobalMinNote;
  std::optional<float> GlobalMaxNote;

  for (int TrackIdx = 0; TrackIdx < TrackCount; ++TrackIdx)
  {
    std::optional<float> TrackMinNote;
    std::optional<float> TrackMaxNote;

    for (int EventIdx = 0; EventIdx < m_MidiFile[TrackIdx].size(); ++EventIdx)
    {
      auto & Event = m_MidiFile[TrackIdx][EventIdx];

      if (Event.isNoteOn())
      {
        m_TrackHasNote[TrackIdx] = true;

        if (Event.seconds < m_FirstNoteTime)
          m_FirstNoteTime = Event.seconds;

        const auto Note = Event.getKeyNumber();

        if (!TrackMinNote.has_value() || Note < TrackMinNote.value())
          TrackMinNote = Note;

        if (!TrackMaxNote.has_value() || Note > TrackMaxNote.value())
          TrackMaxNote = Note;
      }

      if (Event.isMeta())
      {
        const std::string EventMessage = Event.getMetaContent().c_str();

        if (!EventMessage.empty())
        {
          if (!m_TrackMetaMessage[TrackIdx].empty())
            m_TrackMetaMessage[TrackIdx].append("\n");
          m_TrackMetaMessage[TrackIdx].append(EventMessage);
        }
      }
    }

    if (TrackMinNote.has_value() && TrackMaxNote.has_value())
    {
      m_TrackNoteRange[TrackIdx] = { TrackMinNote.value(), TrackMaxNote.value() };

      if (!GlobalMinNote.has_value() || TrackMinNote.value() < GlobalMinNote.value())
        GlobalMinNote = TrackMinNote.value();

      if (!GlobalMaxNote.has_value() || TrackMaxNote.value() > GlobalMaxNote.value())
        GlobalMaxNote = TrackMaxNote.value();
    }
  }

  if (GlobalMinNote.has_value() && GlobalMaxNote.has_value())
  {
    m_Anim.MinNote = GlobalMinNote.value();
    m_Anim.MaxNote = GlobalMaxNote.value();
  }

  m_Time = m_FirstNoteTime - 0.4;

  WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
  CloseHandle(ShExecInfo.hProcess);
}

bool MidiVisualization::IsFileProcessing()
{
  if (!m_ProcessFileFuture.valid())
    return false;

  if (m_ProcessFileFuture.wait_for(std::chrono::milliseconds{ 0 }) == std::future_status::ready)
  {
    m_ProcessFileFuture.get();
    m_IsProcessed = true;
  }

  return m_ProcessFileFuture.valid();
}

void MidiVisualization::StartPlaying()
{
  std::string TempFile = FILES_DIR + "\\" + TEMP_FILE;
  PlaySoundA(TempFile.c_str(), NULL, SND_ASYNC);
  m_Time = m_FirstNoteTime - 0.4;
  m_IsPlaying = true;
}

void MidiVisualization::StopPlaying()
{
  PlaySoundA(NULL, NULL, SND_ASYNC);
  m_IsPlaying = false;
}

void MidiVisualization::RescanDirectory()
{
  StopPlaying();
  m_IsProcessed = false;
  m_FileName.clear();
  m_DirectoryFiles.clear();

  for (const auto & Entry : std::filesystem::directory_iterator(FILES_DIR))
  {
    const auto & Path = Entry.path();
    if (Path.has_extension() && Path.extension() == ".mid")
      m_DirectoryFiles.emplace_back(Path.filename().string());
  }
}
