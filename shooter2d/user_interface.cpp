#include <iomanip>
#include "user_interface.h"

using namespace Shooter;

void FrameUI::PutText()
{
	Text.str("");
	Text << "FPS " << std::fixed << std::setprecision(3) << Time->GetAverageOfFPS();
}