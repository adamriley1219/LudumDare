#pragma once
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Rgba.hpp"
#include <string>

//--------------------------------------------------------------------------

struct AABB2;
struct Vec4;
struct Vec2;
class UIRadioGroup;
class Texture2D;

//--------------------------------------------------------------------------

class UIWidget
{
public:
	UIWidget(); 
	virtual ~UIWidget(); // virtual dctr - why?  Baseline this should kill all my children; 

	virtual void UpdateBounds( const AABB2& container ); 
	virtual void ProcessInput( Event& evt ); // handles input - may consume the event (but it is still passed about to help update state)
	virtual void Render() const; // assumes a camera has already been set; 

	UIWidget* AddChild( UIWidget *widget ); 
	void RemoveChild( UIWidget *widget ); 

	void SetRadioGroup( UIRadioGroup *group ); // adds or removes me from a group

											   // accessors
	inline Vec2 GetWorldPosition() const      { return m_position; }
	inline AABB2 GetWorldBounds() const       { return m_worldBounds; }

public:
	// templated helper I like having
	template <typename T>
	T* CreateChild() 
	{
		T* widget = new T();
		widget->m_parent = this;
		m_children.push_back( widget );
		return widget;
	}

protected:
	// helpers; 
	void UpdateChildrenBounds(); // will update children using my bounds as their container
	void ProcessChildrenInput( Event &evt ); // update input - process backwards through the list (things on top get first crack)
	void RenderChildren() const;  // render children - process forward (later things show up on top)
	void DestroyChildren(); // be sure to kill your children when you go... programming is dark; 

public:
	// heirarchy information
	UIWidget *m_parent         = nullptr; 
	std::vector<UIWidget*> m_children; 

	// human settable independent variables; 
	Vec4 m_virtualPosition     = Vec4( .5f, .5f, 0.0f, 0.0f );     // centered relative to container
	Vec4 m_virtualSize         = Vec4( 1.0f, 1.0f, 0.0f, 0.0f );   // stretch to 100% of the screen 
	Vec2 m_pivot               = Vec2( .5f, .5f );                 // centered

																   // derived from the above;
	Vec2 m_position; 
	AABB2 m_worldBounds; 

	// Misc Data
	unsigned int m_uiFlags   = 0U;       // some state tracking; 
	UIRadioGroup *m_radioGroup = nullptr;  // which radio group do I belong to?
}; 

//--------------------------------------------------------------------------

class UICanvas : public UIWidget
{
public:
	UICanvas();
	~UICanvas();

	virtual void Render() const;
	virtual void UpdateBounds( const AABB2& container );

	Rgba m_fillColor = Rgba::INVISABLE;
	Rgba m_boarderColor = Rgba::INVISABLE;
	float m_boarderThickness = 0.0f;
	float m_boarderScaleWithParent = true;

private:
	AABB2 m_boarderBounds;

};

//------------------------------------------------------------------------
//------------------------------------------------------------------------
class UILabel : public UIWidget
{
public:
	UILabel();
	~UILabel();

	virtual void Render() const;

	Rgba m_color = Rgba::WHITE;
	std::string m_text = "";

	std::string m_font = "SquirrelFixedFont";

}; 



enum eButtonSelectState
{
	BUTTON_STATE_NUTRAL,
	BUTTON_STATE_SELECTED,
	BUTTON_STATE_HOVERED
};

//------------------------------------------------------------------------
class UIButton : public UIWidget
{
public:
	UIButton();
	~UIButton();

	virtual void Render() const;
	virtual void UpdateBounds( const AABB2& container );
	virtual void ProcessInput( Event& evt );
	void Click();


	Rgba m_hoveredColor = Rgba::WHITE;
	Rgba m_selectedColor = Rgba::WHITE;
	Rgba m_nutralColor = Rgba::WHITE;

	Rgba m_nutralBoarderColor = Rgba::WHITE;
	Rgba m_hoveredBoarderColor = Rgba::WHITE;
	Rgba m_selectedBoarderColor = Rgba::WHITE;

	float m_boarderThickness = 0.0f;
	float m_boarderScaleWithParent = true;

	bool m_useText = true;
	std::string m_text = "";
	std::string m_font = "SquirrelFixedFont";

	std::string m_texturePath = "";


	std::string m_eventOnClick = "UNMAPPED_EVENT";

	eButtonSelectState m_state = BUTTON_STATE_NUTRAL;

private:
	AABB2 m_boarderBounds;

}; 

//------------------------------------------------------------------------
//------------------------------------------------------------------------
// class UISlider : public UIWidget
// {
// 	void Change( float value ) 
// 	{
// 		m_value = value; 
// 		// whatever other updating needed for visual reasons;
// 
// 		Event evt( m_eventOnChange ); 
// 		evt.m_args.Add( "value", m_value ); // add this AS A FLOAT
// 		evt.m_args.Add( "source", this );   //  who triggered this event; 
// 
// 		EventFire( evt ); 
// 	}
// 
// 	std::string m_eventOnChange = "changeRadius"; 
// }; 

//------------------------------------------------------------------------
//------------------------------------------------------------------------
// A radio group allows for only a single item in the group 
// to be selected at a time - ie, mutually exclusive selection
class UIRadioGroup
{
	friend class UIWidget;

public:
	std::vector<UIWidget*> m_widgets; 

}; 
