#ifndef GUI_HUD_H__
#define GUI_HUD_H__

#include "GUI/Frame.h"
#include "GUI/TextureFrame.h"
#include "GUI/NumberFrame.h"
#include "Core/EKeyUp.h"
#include "Game/EScoreEvent.h"
#include "Game/GUI/FPSCounter.h"

namespace dd
{
namespace GUI
{

class HUD : public Frame
{
public:
	HUD(Frame* parent, std::string name)
		: Frame(parent, name)
	{
		Width = parent->Width;
		Height = parent->Height;

		m_LevelIndicator = new GUI::TextureFrame(this, "HUDLevelIndicator");
		m_LevelIndicator->SetTexture("Textures/GUI/HUD/LevelIndicatorBG.png");

		m_AreaNumberFrame = new GUI::NumberFrame(m_LevelIndicator, "HUDScoreNumberFrameawdawdwa");
		m_AreaNumberFrame->X = 132;
		m_AreaNumberFrame->Y = 19;
		m_AreaNumberFrame->SetNumber(6);
		m_StageNumberFrame = new GUI::NumberFrame(m_LevelIndicator, "HUDScoreNumberFrameawdawdwa");
		m_StageNumberFrame->X = 165;
		m_StageNumberFrame->Y = 19;
		m_StageNumberFrame->SetNumber(5);

		m_ScoreIndicator = new GUI::TextureFrame(this, "HUDScoreIndicator");
		m_ScoreIndicator->SetTexture("Textures/GUI/HUD/ScoreIndicatorBG.png");
		m_ScoreIndicator->SetRight(Right());
		m_ScoreNumberFrame = new GUI::NumberFrame(m_ScoreIndicator, "HUDScoreNumberFrame");
		m_ScoreNumberFrame->X = 15;
		m_ScoreNumberFrame->Y = 21;

		m_FPSCounter = new GUI::FPSCounter(this, "FPSCounter");

		EVENT_SUBSCRIBE_MEMBER(m_EScore, &HUD::OnScore);
	}

private:
	TextureFrame* m_LevelIndicator = nullptr;
	TextureFrame* m_ScoreIndicator = nullptr;
	NumberFrame* m_AreaNumberFrame = nullptr;
	NumberFrame* m_StageNumberFrame = nullptr;
	NumberFrame* m_ScoreNumberFrame = nullptr;
	FPSCounter* m_FPSCounter = nullptr;

	EventRelay<Frame, Events::ScoreEvent> m_EScore;

	bool OnScore(const Events::ScoreEvent& event)
	{
		m_ScoreNumberFrame->SetNumber(m_ScoreNumberFrame->Number() + event.Score);
		return true;
	}
};

}
}
#endif

