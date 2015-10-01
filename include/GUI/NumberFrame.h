#ifndef GUI_NUMBERFRAME_H__
#define GUI_NUMBERFRAME_H__

#include <unordered_map>

#include "GUI/TextureFrame.h"

namespace dd
{
namespace GUI
{

class NumberFrame : public Frame
{
public:
	NumberFrame(Frame* parent, std::string name)
		: Frame(parent, name)
	{
		m_NumberTextures[0] = "Textures/GUI/Numbers/N0.png";
		m_NumberTextures[1] = "Textures/GUI/Numbers/N1.png";
		m_NumberTextures[2] = "Textures/GUI/Numbers/N2.png";
		m_NumberTextures[3] = "Textures/GUI/Numbers/N3.png";
		m_NumberTextures[4] = "Textures/GUI/Numbers/N4.png";
		m_NumberTextures[5] = "Textures/GUI/Numbers/N5.png";
		m_NumberTextures[6] = "Textures/GUI/Numbers/N6.png";
		m_NumberTextures[7] = "Textures/GUI/Numbers/N7.png";
		m_NumberTextures[8] = "Textures/GUI/Numbers/N8.png";
		m_NumberTextures[9] = "Textures/GUI/Numbers/N9.png";

		// Create number frame if missing
//		for (int i = 0; i < 10; i++) {
//			auto frame = new GUI::TextureFrame(this, "NumberFrameDigit");
//			frame->SetTexture(m_NumberTextures[0]);
//			frame->DisableScissor();
//			// Position frame based on alignment
//			if (i > 0) {
//				frame->SetLeft(m_NumberFrames[0]->Right());
//			}
//			Width += frame->Width;
//			if (Height < frame->Height) {
//				Height = frame->Height;
//			}
//			m_NumberFrames.push_front(frame);
//		}

		SetNumber(0);
	}

	int Number() const { return m_Number; }
	void SetNumber(int number)
	{
		m_Number = number;

		// Clear number textures
		for (auto& frame : m_NumberFrames) {
			frame->SetTexture("");
		}

		Width = 0;
		Height = 0;

		std::string digits = std::to_string(number);

		Width = 0;
		Height = 0;

		std::string digits = std::to_string(number);


		int i = 0;
		for (char& c : digits) {
			// Create number frame if missing
			if (i >= m_NumberFrames.size()) {
				auto frame = new GUI::TextureFrame(this, "NumberFrameDigit");
				if (i > 0) {
					frame->SetLeft(m_NumberFrames[i - 1]->Right());
				}
				frame->DisableScissor();
				m_NumberFrames.push_back(frame);
			}

			// Update value
			std::string texture = "Textures/Core/ErrorTexture.png";
			if (m_CharacterTextures.find(c) != m_CharacterTextures.end()) {
				texture = m_CharacterTextures.at(c);
			}
			m_NumberFrames[i]->SetTexture(m_CharacterTextures.at(c));
			Width += m_NumberFrames[i]->Width;
			if (Height < m_NumberFrames[i]->Height) {
				Height = m_NumberFrames[i]->Height;
			}

			i++;
		}

		int n = 0;
		do {
			// Create number frame if missing
			if (m_NumberFrames.size() <= n) {
				auto frame = new GUI::TextureFrame(this, "NumberFrameDigit");
				frame->DisableScissor();
				m_NumberFrames.push_back(frame);
			}

			int digit = number % 10;

			// Update value
			m_NumberFrames[n]->SetTexture(m_NumberTextures[digit]);
			Width += m_NumberFrames[n]->Width;
			if (Height < m_NumberFrames[n]->Height) {
				Height = m_NumberFrames[n]->Height;
			}
			// Position frame based on alignment
			if (n > 0) {
				if (m_LeftAligned) {
					m_NumberFrames[n]->SetLeft(m_NumberFrames[n - 1]->Right());
				} else {
					m_NumberFrames[n]->SetRight(m_NumberFrames[n - 1]->Left());
				}
			}

			ss << digit;

			number /= 10;
			n++;
		} while (number > 0);

		LOG_DEBUG("Number: %s", ss.str().c_str());
	}

	void SetLeftAlign() { m_LeftAligned = true; }
	void SetRightAlign() { m_LeftAligned = false; }

	virtual void Update(double dt) override
	{

	}

private:
	int m_Number = 0;
	bool m_LeftAligned = true;

	std::string m_NumberTextures[10];
	std::unordered_map<char, std::string> m_CharacterTextures;
	std::deque<TextureFrame*> m_NumberFrames;
};

}
}
#endif

