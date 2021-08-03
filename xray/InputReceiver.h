#pragma once

struct IInputReceiver;

struct IInputReceiverVtbl
{
  void (__thiscall *IR_OnDeactivate)(IInputReceiver *_this);
  void (__thiscall *IR_OnActivate)(IInputReceiver *_this);
  void (__thiscall *IR_OnMousePress)(IInputReceiver *_this, int);
  void (__thiscall *IR_OnMouseRelease)(IInputReceiver *_this, int);
  void (__thiscall *IR_OnMouseHold)(IInputReceiver *_this, int);
  void (__thiscall *IR_OnMouseWheel)(IInputReceiver *_this, int);
  void (__thiscall *IR_OnMouseMove)(IInputReceiver *_this, int, int);
  void (__thiscall *IR_OnMouseStop)(IInputReceiver *_this, int, int);
  void (__thiscall *IR_OnKeyboardPress)(IInputReceiver *_this, int);
  void (__thiscall *IR_OnKeyboardRelease)(IInputReceiver *_this, int);
  void (__thiscall *IR_OnKeyboardHold)(IInputReceiver *_this, int);
};

struct IInputReceiver
{
  IInputReceiverVtbl *vfptr;
};

