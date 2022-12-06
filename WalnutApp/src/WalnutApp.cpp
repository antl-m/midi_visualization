#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "MidiVisualization.h"

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
  Walnut::ApplicationSpecification spec;
  spec.Name = "Walnut Example";

  Walnut::Application* app = new Walnut::Application(spec);
  app->PushLayer<MidiVisualization>();
  app->SetMenubarCallback([app]()
    {
      if (ImGui::BeginMenu("File"))
      {
        if (ImGui::MenuItem("Exit"))
        {
          app->Close();
        }
        ImGui::EndMenu();
      }
    });
  return app;
}