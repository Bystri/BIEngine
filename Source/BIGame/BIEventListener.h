#pragma once

#include <glm/glm.hpp>

#include "../BIEngine/EventManager/EventManager.h"
#include "../BIEngine/Actors/Actor.h"

#include "PlayerManager/PlayerManager.h"
#include "BIGameController.h"

class EvtData_OnKeyEvent : public BIEngine::BaseEventData {
public:
   enum class Key {
      UNKNOWN = -1,

      SPACE = 32,
      APOSTROPHE = 39, /* ' */
      COMMA = 44,      /* , */
      MINUS = 45,      /* - */
      PERIOD = 46,     /* . */
      SLASH = 47,      /* / */
      KEY_0 = 48,
      KEY_1 = 49,
      KEY_2 = 50,
      KEY_3 = 51,
      KEY_4 = 52,
      KEY_5 = 53,
      KEY_6 = 54,
      KEY_7 = 55,
      KEY_8 = 56,
      KEY_9 = 57,
      SEMICOLON = 59, /* ; */
      EQUAL = 61,     /* = */
      A = 65,
      B = 66,
      C = 67,
      D = 68,
      E = 69,
      F = 70,
      G = 71,
      H = 72,
      I = 73,
      J = 74,
      K = 75,
      L = 76,
      M = 77,
      N = 78,
      O = 79,
      P = 80,
      Q = 81,
      R = 82,
      S = 83,
      T = 84,
      U = 85,
      V = 86,
      W = 87,
      X = 88,
      Y = 89,
      Z = 90,
      LEFT_BRACKET = 91,  /* [ */
      BACKSLASH = 92,     /* \ */
      RIGHT_BRACKET = 93, /* ] */
      GRAVE_ACCENT = 96,  /* ` */
      WORLD_1 = 161,      /* non-US #1 */
      WORLD_2 = 162,      /* non-US #2 */

      /* Function keys */
      ESCAPE = 256,
      ENTER = 257,
      TAB = 258,
      BACKSPACE = 259,
      INSERT = 260,
      DEL = 261,
      RIGHT = 262,
      LEFT = 263,
      DOWN = 264,
      UP = 265,
      PAGE_UP = 266,
      PAGE_DOWN = 267,
      HOME = 268,
      END = 269,
      CAPS_LOCK = 280,
      SCROLL_LOCK = 281,
      NUM_LOCK = 282,
      PRINT_SCREEN = 283,
      PAUSE = 284,
      F1 = 290,
      F2 = 291,
      F3 = 292,
      F4 = 293,
      F5 = 294,
      F6 = 295,
      F7 = 296,
      F8 = 297,
      F9 = 298,
      F10 = 299,
      F11 = 300,
      F12 = 301,
      F13 = 302,
      F14 = 303,
      F15 = 304,
      F16 = 305,
      F17 = 306,
      F18 = 307,
      F19 = 308,
      F20 = 309,
      F21 = 310,
      F22 = 311,
      F23 = 312,
      F24 = 313,
      F25 = 314,
      KP_0 = 320,
      KP_1 = 321,
      KP_2 = 322,
      KP_3 = 323,
      KP_4 = 324,
      KP_5 = 325,
      KP_6 = 326,
      KP_7 = 327,
      KP_8 = 328,
      KP_9 = 329,
      KP_DECIMAL = 330,
      KP_DIVIDE = 331,
      KP_MULTIPLY = 332,
      KP_SUBTRACT = 333,
      KP_ADD = 334,
      KP_ENTER = 335,
      KP_EQUAL = 336,
      LEFT_SHIFT = 340,
      LEFT_CONTROL = 341,
      LEFT_ALT = 342,
      LEFT_SUPER = 343,
      RIGHT_SHIFT = 344,
      RIGHT_CONTROL = 345,
      RIGHT_ALT = 346,
      RIGHT_SUPER = 347,
      MENU = 348
   };

public:
   EvtData_OnKeyEvent()
      : m_playerId(-1), m_key(Key::UNKNOWN)
   {
   }

   EvtData_OnKeyEvent(uint32_t playerId, Key key)
      : m_playerId(playerId), m_key(key)
   {
   }

   uint32_t GetPlayerId() const
   {
      return m_playerId;
   }

   Key GetKey() const
   {
      return m_key;
   }

private:
   uint32_t m_playerId;
   Key m_key;
};

class EvtData_OnKeyDown : public EvtData_OnKeyEvent {
public:
   static const BIEngine::EventType sk_EventType;

   virtual const BIEngine::EventType& GetEventType() const
   {
      return sk_EventType;
   }

   EvtData_OnKeyDown() = default;

   EvtData_OnKeyDown(uint32_t m_playerId, Key key)
      : EvtData_OnKeyEvent(m_playerId, key)
   {
   }

   virtual BIEngine::IEventDataPtr Copy() const
   {
      return BIEngine::MakeShared<EvtData_OnKeyDown>(GetPlayerId(), GetKey());
   }

   virtual const char* GetName() const
   {
      return "EvtData_OnKeyDown";
   }
};

class EvtData_OnKeyUp : public EvtData_OnKeyEvent {
public:
   static const BIEngine::EventType sk_EventType;

   virtual const BIEngine::EventType& GetEventType() const
   {
      return sk_EventType;
   }

   EvtData_OnKeyUp() = default;

   EvtData_OnKeyUp(uint32_t m_playerId, Key key)
      : EvtData_OnKeyEvent(m_playerId, key)
   {
   }

   virtual BIEngine::IEventDataPtr Copy() const
   {
      return BIEngine::MakeShared<EvtData_OnKeyUp>(GetPlayerId(), GetKey());
   }

   virtual const char* GetName() const
   {
      return "EvtData_OnKeyUp";
   }
};

class EvtData_OnPointerMove : public BIEngine::BaseEventData {
public:
   static const BIEngine::EventType sk_EventType;

   virtual const BIEngine::EventType& GetEventType() const
   {
      return sk_EventType;
   }

   EvtData_OnPointerMove() = default;

   EvtData_OnPointerMove(uint32_t playerId, const BIEngine::IPointerHandler::Point& pointerPos)
      : m_playerId(playerId), m_pointerPos(pointerPos)
   {
   }

   uint32_t GetPlayerId() const
   {
      return m_playerId;
   }

   const BIEngine::IPointerHandler::Point& GetPointerPos() const
   {
      return m_pointerPos;
   }

   virtual BIEngine::IEventDataPtr Copy() const
   {
      return BIEngine::MakeShared<EvtData_OnPointerMove>(m_playerId, m_pointerPos);
   }

   virtual const char* GetName() const
   {
      return "EvtData_OnPointerMove";
   }

private:
   uint32_t m_playerId = -1;
   BIEngine::IPointerHandler::Point m_pointerPos = BIEngine::IPointerHandler::Point{0.0f, 0.0f};
};

class EvtData_OnPointerButtonDown : public BIEngine::BaseEventData {
public:
   static const BIEngine::EventType sk_EventType;

   virtual const BIEngine::EventType& GetEventType() const
   {
      return sk_EventType;
   }

   EvtData_OnPointerButtonDown() = default;

   EvtData_OnPointerButtonDown(uint32_t playerId, const BIEngine::IPointerHandler::Point& pointerPos, const BIGameController::MouseButton mouseButton)
      : m_playerId(playerId), m_pointerPos(pointerPos), m_mouseButton(mouseButton)
   {
   }

   uint32_t GetPlayerId() const
   {
      return m_playerId;
   }

   const BIEngine::IPointerHandler::Point& GetPointerPos() const
   {
      return m_pointerPos;
   }

   BIGameController::MouseButton GetMouseButton() const
   {
      return m_mouseButton;
   }

   virtual BIEngine::IEventDataPtr Copy() const
   {
      return BIEngine::MakeShared<EvtData_OnPointerButtonDown>(m_playerId, m_pointerPos, m_mouseButton);
   }

   virtual const char* GetName() const
   {
      return "EvtData_OnPointerButtonDown";
   }

private:
   uint32_t m_playerId = -1;
   BIEngine::IPointerHandler::Point m_pointerPos = BIEngine::IPointerHandler::Point{0.0f, 0.0f};
   BIGameController::MouseButton m_mouseButton;
};

class EvtData_OnPointerButtonUp : public BIEngine::BaseEventData {
public:
   static const BIEngine::EventType sk_EventType;

   virtual const BIEngine::EventType& GetEventType() const
   {
      return sk_EventType;
   }

   EvtData_OnPointerButtonUp() = default;

   EvtData_OnPointerButtonUp(uint32_t playerId, const BIEngine::IPointerHandler::Point& pointerPos, const BIGameController::MouseButton mouseButton)
      : m_playerId(playerId), m_pointerPos(pointerPos), m_mouseButton(mouseButton)
   {
   }

   uint32_t GetPlayerId() const
   {
      return m_playerId;
   }

   const BIEngine::IPointerHandler::Point& GetPointerPos() const
   {
      return m_pointerPos;
   }

   BIGameController::MouseButton GetMouseButton() const
   {
      return m_mouseButton;
   }

   virtual BIEngine::IEventDataPtr Copy() const
   {
      return BIEngine::MakeShared<EvtData_OnPointerButtonUp>(m_playerId, m_pointerPos, m_mouseButton);
   }

   virtual const char* GetName() const
   {
      return "EvtData_OnPointerButtonUp";
   }

private:
   uint32_t m_playerId = -1;
   BIEngine::IPointerHandler::Point m_pointerPos = BIEngine::IPointerHandler::Point{0.0f, 0.0f};
   BIGameController::MouseButton m_mouseButton;
};

class EvtData_Player_Created : public BIEngine::BaseEventData {
public:
   static const BIEngine::EventType sk_EventType;

   explicit EvtData_Player_Created(BIEngine::SharedPtr<Player> pPlayer)
      : m_pPlayer(pPlayer)
   {
   }

   virtual const BIEngine::EventType& GetEventType() const
   {
      return sk_EventType;
   }

   virtual BIEngine::IEventDataPtr Copy() const
   {
      return BIEngine::MakeShared<EvtData_Player_Created>(m_pPlayer);
   }

   virtual const char* GetName() const
   {
      return "EvtData_Player_Created";
   }

   BIEngine::SharedPtr<Player> GetPlayer() const { return m_pPlayer; }

private:
   BIEngine::SharedPtr<Player> m_pPlayer;
};

class EvtData_Player_BeforeDestroyed : public BIEngine::BaseEventData {
public:
   static const BIEngine::EventType sk_EventType;

   explicit EvtData_Player_BeforeDestroyed(BIEngine::SharedPtr<Player> pPlayer)
      : m_pPlayer(pPlayer)
   {
   }

   virtual const BIEngine::EventType& GetEventType() const
   {
      return sk_EventType;
   }

   virtual BIEngine::IEventDataPtr Copy() const
   {
      return BIEngine::MakeShared<EvtData_Player_BeforeDestroyed>(m_pPlayer);
   }

   virtual const char* GetName() const
   {
      return "EvtData_Player_BeforeDestroyed";
   }

   BIEngine::SharedPtr<Player> GetPlayer() const { return m_pPlayer; }

private:
   BIEngine::SharedPtr<Player> m_pPlayer;
};

class EvtData_PlayerActor_Created : public BIEngine::BaseEventData {
public:
   static const BIEngine::EventType sk_EventType;

   EvtData_PlayerActor_Created(int playerId, BIEngine::ActorId id)
      : m_playerId(playerId), m_actorId(id)
   {
   }

   virtual const BIEngine::EventType& GetEventType(void) const
   {
      return sk_EventType;
   }

   virtual BIEngine::IEventDataPtr Copy(void) const
   {
      return BIEngine::MakeShared<EvtData_PlayerActor_Created>(m_playerId, m_actorId);
   }

   virtual const char* GetName(void) const
   {
      return "EvtData_PlayerActor_Created";
   }

   int GetPlayerId(void) const { return m_playerId; }

   BIEngine::ActorId GetActorId(void) const { return m_actorId; }

private:
   int m_playerId;
   BIEngine::ActorId m_actorId = BIEngine::Actor::INVALID_ACTOR_ID;
};

class EvtData_Move : public BIEngine::BaseEventData {
public:
   static const BIEngine::EventType sk_EventType;

   EvtData_Move()
      : m_playerId(PlayerManager::INVALID_PLAYER_ID), m_desiredHorizontalAmount(0.0f), m_desiredVerticalAmount(0.0f)
   {
   }

   EvtData_Move(PlayerId playerId, float desiredHorizontalAmount, float desiredVerticalAmount)
      : m_playerId(playerId), m_desiredHorizontalAmount(desiredHorizontalAmount), m_desiredVerticalAmount(desiredVerticalAmount)
   {
   }

   virtual const BIEngine::EventType& GetEventType(void) const
   {
      return sk_EventType;
   }

   virtual BIEngine::IEventDataPtr Copy(void) const
   {
      return BIEngine::MakeShared<EvtData_Move>(m_playerId, m_desiredHorizontalAmount, m_desiredVerticalAmount);
   }

   virtual const char* GetName(void) const
   {
      return "EvtData_Move";
   }

   virtual void Write(BIEngine::OutputMemoryBitStream& out) const override
   {
      Serialize(out, m_playerId);
      Serialize(out, m_desiredHorizontalAmount);
      Serialize(out, m_desiredVerticalAmount);
   }

   virtual void Read(BIEngine::InputMemoryBitStream& in) override
   {
      Deserialize(in, m_playerId);
      Deserialize(in, m_desiredHorizontalAmount);
      Deserialize(in, m_desiredVerticalAmount);
   }

   PlayerId GetPlayerId() const { return m_playerId; };

   float GetDesiredHorizontalAmount() const { return m_desiredHorizontalAmount; }

   float GetDesiredVerticalAmount() const { return m_desiredVerticalAmount; }

private:
   PlayerId m_playerId;

   float m_desiredHorizontalAmount = 0.0f;
   float m_desiredVerticalAmount = 0.0f;
};

class EvtData_Turn : public BIEngine::BaseEventData {
public:
   static const BIEngine::EventType sk_EventType;

   EvtData_Turn() = default;

   EvtData_Turn(PlayerId playerId, const glm::vec2& desiredDir)
      : m_playerId(playerId), m_desiredDir(desiredDir)
   {
   }

   virtual const BIEngine::EventType& GetEventType(void) const
   {
      return sk_EventType;
   }

   virtual BIEngine::IEventDataPtr Copy(void) const
   {
      return BIEngine::MakeShared<EvtData_Turn>(m_playerId, m_desiredDir);
   }

   virtual const char* GetName(void) const
   {
      return "EvtData_Turn";
   }

   virtual void Write(BIEngine::OutputMemoryBitStream& out) const override
   {
      Serialize(out, m_playerId);
      out.WriteBytes(&m_desiredDir.x, sizeof(m_desiredDir));
   }

   virtual void Read(BIEngine::InputMemoryBitStream& in) override
   {
      Deserialize(in, m_playerId);
      in.ReadBytes(&m_desiredDir.x, sizeof(m_desiredDir));
   }

   PlayerId GetPlayerId() const { return m_playerId; };

   const glm::vec2& GetDesiredDir() const { return m_desiredDir; }

private:
   PlayerId m_playerId = PlayerManager::INVALID_PLAYER_ID;

   glm::vec2 m_desiredDir = glm::vec2(0.0f);
};

class EvtData_PrimaryAttack : public BIEngine::BaseEventData {
public:
   static const BIEngine::EventType sk_EventType;

   EvtData_PrimaryAttack() = default;

   EvtData_PrimaryAttack(PlayerId playerId)
      : m_playerId(playerId)
   {
   }

   virtual const BIEngine::EventType& GetEventType(void) const
   {
      return sk_EventType;
   }

   virtual BIEngine::IEventDataPtr Copy(void) const
   {
      return BIEngine::MakeShared<EvtData_PrimaryAttack>(m_playerId);
   }

   virtual const char* GetName(void) const
   {
      return "EvtData_PrimaryAttack";
   }

   virtual void Write(BIEngine::OutputMemoryBitStream& out) const override
   {
      Serialize(out, m_playerId);
   }

   virtual void Read(BIEngine::InputMemoryBitStream& in) override
   {
      Deserialize(in, m_playerId);
   }

   PlayerId GetPlayerId() const { return m_playerId; };

private:
   PlayerId m_playerId = PlayerManager::INVALID_PLAYER_ID;
};

class EvtData_PlayerCommandMoveTo : public BIEngine::BaseEventData {
public:
   static const BIEngine::EventType sk_EventType;

   EvtData_PlayerCommandMoveTo()
      : m_playerId(PlayerManager::INVALID_PLAYER_ID), m_pos()
   {
   }

   EvtData_PlayerCommandMoveTo(PlayerId playerId, const glm::vec3& pos)
      : m_playerId(playerId), m_pos(pos)
   {
   }

   virtual const BIEngine::EventType& GetEventType(void) const
   {
      return sk_EventType;
   }

   virtual BIEngine::IEventDataPtr Copy(void) const
   {
      return BIEngine::MakeShared<EvtData_PlayerCommandMoveTo>(m_playerId, m_pos);
   }

   virtual const char* GetName(void) const
   {
      return "EvtData_PlayerCommandMoveTo";
   }

   virtual void Write(BIEngine::OutputMemoryBitStream& out) const override
   {
      Serialize(out, m_playerId);
      Serialize(out, m_pos.x);
      Serialize(out, m_pos.y);
      Serialize(out, m_pos.z);
   }

   virtual void Read(BIEngine::InputMemoryBitStream& in) override
   {
      Deserialize(in, m_playerId);
      Deserialize(in, m_pos.x);
      Deserialize(in, m_pos.y);
      Deserialize(in, m_pos.z);
   }

   PlayerId GetPlayerId() const { return m_playerId; };

   const glm::vec3& GetPosToMove() const { return m_pos; }

private:
   PlayerId m_playerId;

   glm::vec3 m_pos;
};

void BIRegisterEvents();
