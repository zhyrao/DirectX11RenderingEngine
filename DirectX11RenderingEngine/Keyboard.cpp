#include "Keyboard.h"

bool Keyboard::KeyIsPressed(unsigned char keyCode) const noexcept
{
	return keystates[keyCode];
}

Keyboard::Event Keyboard::ReadKey() noexcept
{
	if (keybuffers.size() > 0u)
	{
		Keyboard::Event e = keybuffers.front();
		keybuffers.pop();
		return e;
	}

	return Keyboard::Event(Event::Type::Invalid, -1);
}

bool Keyboard::KeyIsEmpty() const noexcept
{
	return keybuffers.empty();
}

void Keyboard::FlushKey() noexcept
{
	keybuffers = std::queue<Event>();
}

char Keyboard::ReadChar() noexcept
{
	if (charbuffers.size() > 0u)
	{
		unsigned char charcode = charbuffers.front();
		charbuffers.pop();
		return charcode;
	}
	return 0;
}

bool Keyboard::CharIsEmpty() const noexcept
{
	return charbuffers.empty();
}

void Keyboard::FlushChar() noexcept
{
	charbuffers = std::queue<char>();
}

void Keyboard::Flush() noexcept
{
	FlushKey();
	FlushChar();
}

void Keyboard::EnableAutoRepeat() noexcept
{
	autorepeatEnabled = true;
}

void Keyboard::DisableAutoRepeat() noexcept
{
	autorepeatEnabled = false;
}

bool Keyboard::AutorepeatIsEnable() const noexcept
{
	return autorepeatEnabled;
}


Keyboard::~Keyboard()
{
}

void Keyboard::OnKeyPressed(unsigned char keyCode) noexcept
{
	keystates[keyCode] = true;
	keybuffers.push(Keyboard::Event(Keyboard::Event::Type::Press, keyCode));
	TrimBuffer(keybuffers);
}

void Keyboard::OnKeyReleased(unsigned char keyCode) noexcept
{
	keystates[keyCode] = false;
	keybuffers.push(Keyboard::Event(Keyboard::Event::Type::Release, keyCode));
	TrimBuffer(keybuffers);
}

void Keyboard::OnChar(char character) noexcept
{
	charbuffers.push(character);
	TrimBuffer(charbuffers);
}

void Keyboard::ClearState() noexcept
{
	keystates.reset();
}

template<typename T>
void Keyboard::TrimBuffer(std::queue<T>& buffer) noexcept
{
	while (buffer.size() > bufferSize)
	{
		buffer.pop();
	}
}
