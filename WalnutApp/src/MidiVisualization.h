#pragma once

#include "Walnut/Layer.h"
#include "MidiFile.h"

#include <future>
#include <vector>
#include <string>

class MidiVisualization : public Walnut::Layer
{
public: // Members

  smf::MidiFile m_MidiFile;
  std::string   m_FileName;

  float m_TrackOffset    = 0;
  float m_PixelPerSecond = 100;
  float m_NoteHeight     = 5;
  float m_Time           = -0.5;
  bool  m_Follow         = false;
  bool  m_IsPlaying      = false;

  std::vector<std::pair<float, float>> m_TrackNoteRange;
  std::vector<std::string>             m_TrackMetaMessage;
  std::vector<bool>                    m_TrackHasNote;
  float                                m_FirstNoteTime = FLT_MAX;

  std::future<void>        m_ProcessFileFuture;
  std::vector<std::string> m_DirectoryFiles;
  bool                     m_IsProcessed;

  struct {

    float MinNote      = -1;
    float MaxNote      = -1;
    float FigureHeight = 100;
    float PixelPerSec  = 300;
    bool  RenderGrid   = false;

  } m_Anim;

private: // Constants

  static const std::string FILES_DIR;
  static const std::string TEMP_FILE;

public: // Walnut::Layer

  void OnAttach() override;

  void OnUIRender() override;

  void OnUpdate(
      float _DeltaTime
    ) override;

public: // Service UI

  void RenderFileControls();

  void RenderMidiContent();

  void RenderAnimation();

private: // Service

  void StartProcessFile(
      const std::string & _FileName
    );

  void ProcessFile(
      const std::string & _FileName
    );

  bool IsFileProcessing();

  void StartPlaying();

  void StopPlaying();

  void RescanDirectory();
};
