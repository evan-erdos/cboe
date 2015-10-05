/*
 *  dlogutil.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 11/05/09.
 *
 */

#ifndef DIALOG_UTIL_H
#define DIALOG_UTIL_H

/// @file
/// A set of utility classes for simple and common dialogs.

#include <string>
#include <vector>
#include <array>
#include <functional>
#include "pictypes.hpp"
#include "soundtool.hpp" // for snd_num_t
#include "pict.hpp"
#include "dialog.hpp"
#include "button.hpp"
#include <boost/optional.hpp>

/// The signature of a record handler for cStrDlog.
typedef std::function<void(cDialog&)> record_callback_t;

/// A simple dialog with one or two long strings, an optional title, and an optional record button.
class cStrDlog {
	static std::string getFileName(short n_strs, ePicType type, bool hasTitle);
	cDialog dlg;
	short sound = -1;
	record_callback_t rec_f;
	bool hasRecord;
	const ePicType type;
	bool onRecord(std::string id);
	bool onDismiss();
public:
	/// Construct a string dialog.
	/// @param str1 The first string.
	/// @param str2 The second string. If left as an empty string, there will be only one string in the dialog.
	/// @param title The title. If left as an empty string, there will be no title.
	/// @param pic The icon to show at the top left.
	/// @param t The type of icon to show.
	/// @param parent Optionally, a parent dialog.
	cStrDlog(std::string str1,std::string str2,std::string title,pic_num_t pic,ePicType t,cDialog* parent = nullptr);
	/// Set a sound to be played when the dialog is shown.
	/// @param num The sound number.
	/// @return This object, for method-call chaining.
	cStrDlog& setSound(snd_num_t num);
	/// Set a record handler.
	/// @param rec The handler.
	/// @return This object, for method-call chaining.
	/// @note Only one record handler can be set at a time. To remove it, set it to null.
	/// @note The presence of the Record button is determined entirely by the presence of a record handler.
	///
	/// A record handler should take one parameter, which is a reference to the dialog.
	/// (That's the cDialog, not the cStrDlog.) It should return void.
	cStrDlog& setRecordHandler(record_callback_t rec);
	/// Reference the cDialog powering this choice dialog, perhaps to customize details of it.
	/// @return A pointer to the dialog.
	cDialog* operator->();
	/// Show the dialog.
	void show();
};

/// A simple dialog that lets you select one of several buttons.
/// This class loads a definition from a file, so there can be any amount of other stuff in the dialog,
/// and the buttons could be arranged in any fashion you want.
class cChoiceDlog {
	cDialog dlg;
protected:
	/// The click handler for the dialog's buttons.
	/// @param me A reference to the current dialog.
	/// @param id The unique key of the clicked control.
	/// @return true, indicating the event should continue.
	bool onClick(cDialog& me, std::string id);
	/// Create a choice dialog, but don't initialize it.
	/// @param p Optionally, a parent dialog.
	explicit cChoiceDlog(cDialog* p = nullptr);
public:
	/// Create a choice dialog with just one button.
	/// @param file The file to load the dialog definition from.
	/// @param p Optionally, a parent dialog.
	/// @note The dialog definition file must include a button whose name attribute is "okay".
	explicit cChoiceDlog(std::string file, cDialog* p = nullptr);
	/// Create a choice dialog with several buttons.
	/// @param file The file to load the dialog definition from.
	/// @param buttons A list of the buttons to handle. All of them must be defined in the file.
	/// @param p Optionally, a parent dialog.
	cChoiceDlog(std::string file, std::vector<std::string> buttons, cDialog* p = nullptr);
	/// Reference the cDialog powering this choice dialog, perhaps to customize details of it.
	/// @return A pointer to the dialog.
	cDialog* operator->();
	/// Show the dialog.
	/// @return The unique key of the clicked button.
	std::string show();
};

/// Basic button type template
struct bbtt {
	eBtnType type;		///< The type of the preset button.
	std::string label;	///< The preset button's label, if any.
	cKey defaultKey;	///< The preset button's default key shortcut, if any.
	std::string name;	///< (optional) A more descriptive name for the button.
};

/// Represents a preset button for use with cThreeChoice.
typedef boost::optional<bbtt> cBasicButtonType;

namespace {cBasicButtonType null_btn = boost::none;}
#ifndef BTNS_DEFINED
extern bbtt basic_buttons[71];
#endif

/// A choice dialog with several strings and up to three buttons.
/// This is the class used for dialogs generated by special nodes.
/// It generates the dialog dynamically from the given input.
/// Note that the dialog is not limited to six strings.
class cThreeChoice : public cChoiceDlog {
	//static std::string getFileName(size_t n_strs);
	cBasicButtonType btns[3];
	unsigned short buttons_right, buttons_top;
	void init_strings(std::vector<std::string>& strings, unsigned short left);
	void init_buttons(cBasicButtonType btn1, cBasicButtonType btn2, cBasicButtonType btn3);
	void init_pict(pic_num_t pic);
	const ePicType type;
public:
	/// Create a dialog with just one button.
	/// @param strings A list of the strings to place in the dialog.
	/// @param button The specification of the button.
	/// @param pic The icon to show at the top left.
	/// @param t The type of the icon.
	/// @param parent Optionally, a parent dialog.
	cThreeChoice(std::vector<std::string>& strings, cBasicButtonType button, pic_num_t pic, ePicType t, cDialog* parent = nullptr);
	/// Create a dialog with up to three buttons.
	/// @param strings A list of the strings to place in the dialog.
	/// @param buttons A list of the button specifications.
	/// @param pic The icon to show at the top left.
	/// @param t The type of the icon.
	/// @param parent Optionally, a parent dialog.
	cThreeChoice(std::vector<std::string>& strings, std::array<cBasicButtonType, 3>& buttons, pic_num_t pic, ePicType t, cDialog* parent = nullptr);
	/// Create a dialog with up to three buttons.
	/// @param strings A list of the strings to place in the dialog.
	/// @param buttons A list of the index of the button; this is an index into available_btns which is in turn used to index basic_buttons.
	/// @param pic The icon to show at the top left.
	/// @param t The type of the icon.
	/// @param parent Optionally, a parent dialog.
	cThreeChoice(std::vector<std::string>& strings, std::array<short, 3>& buttons, pic_num_t pic, ePicType t, cDialog* parent = nullptr);
	/// @copydoc cChoiceDlog::show()
	/// @note The unique key in this case is the label specified in the button specification.
	std::string show();
};

/// A dialog that presents a list of strings with LEDs and allows you to choose one.
/// The list may span several pages.
class cStringChoice {
	static const size_t per_page = 40;
	cDialog dlg;
	bool onLeft();
	bool onRight();
	bool onCancel(cDialog& me);
	bool onOkay(cDialog& me);
	bool onSelect(bool losing);
	void attachHandlers();
	void fillPage();
	std::vector<std::string> strings;
	size_t page, cur;
	cLedGroup* leds;
	std::function<void(cStringChoice&,int)> select_handler;
public:
	/// Initializes a dialog from a list of strings.
	/// @param strs A list of all strings in the dialog.
	/// @param title The title to show in the dialog.
	/// @param parent Optionally, a parent dialog.
	explicit cStringChoice(std::vector<std::string>& strs, std::string title, cDialog* parent = nullptr);
	/// Initializes a dialog from an iterator pair.
	/// @param begin An iterator to the first string in the dialog.
	/// @param end An iterator to one past the last string in the dialog.
	/// @param title The title to show in the dialog.
	/// @param parent Optionally, a parent dialog.
	/// @note Currently, only vector iterators are supported.
	cStringChoice(std::vector<std::string>::iterator begin, std::vector<std::string>::iterator end, std::string title, cDialog* parent = nullptr);
	/// Attach a handler to be called when the selected item changes.
	/// @param f A function that takes a reference to the dialog and the index of the newly selected item.
	void attachSelectHandler(std::function<void(cStringChoice&,int)> f);
	/// Reference the cDialog powering this choice dialog, perhaps to customize details of it.
	/// @return A pointer to the dialog.
	cDialog* operator->();
	/// Show the dialog.
	/// @param selectedIndex The index of the string that should be initially selected when the dialog is shown.
	/// @return The index of the newly selected string; if the user cancelled, this will be equal to selectedIndex.
	/// If initialized from an iterator range, this will be relative to begin.
	size_t show(size_t selectedIndex);
};

/// Like cStringChoice, but presents a list of icons rather than strings.
class cPictChoice {
	static const size_t per_page = 36;
	bool didAccept;
	cDialog dlg;
	void attachHandlers();
	bool onLeft();
	bool onRight();
	bool onCancel();
	bool onOkay();
	bool onSelect(bool losing);
	void fillPage();
	std::vector<std::pair<pic_num_t,ePicType>> picts;
	size_t page, cur;
	cLedGroup* leds;
	std::function<void(cPictChoice&,int)> select_handler;
public:
	/// Initializes a dialog from a list of icons.
	/// @param pics A list of all icons in the dialog.
	/// @param t The type of icons to show; all icons are assumed to be of the same type.
	/// @param parent Optionally, a parent dialog.
	cPictChoice(const std::vector<pic_num_t>& pics, ePicType t, cDialog* parent = nullptr);
	/// Initializes a dialog from a list of icons.
	/// @param pics A list of all icons in the dialog as {num,type} pairs.
	/// @param parent Optionally, a parent dialog.
	cPictChoice(const std::vector<std::pair<pic_num_t,ePicType>>& pics, cDialog* parent = nullptr);
	/// Initializes a dialog from an iterator pair.
	/// @param begin An iterator to the first icon in the dialog.
	/// @param end An iterator to one past the last icon in the dialog.
	/// @param t The type of icons to show; all icons are assumed to be of the same type.
	/// @param parent Optionally, a parent dialog.
	cPictChoice(std::vector<pic_num_t>::const_iterator begin, std::vector<pic_num_t>::const_iterator end, ePicType t, cDialog* parent = nullptr);
	/// Initializes a dialog from an index pair.
	/// @param first The number of the first icon in the dialog.
	/// @param last The number of the last icon in the dialog.
	/// @param t The type of icons to show; all icons are assumed to be of the same type.
	/// @param parent Optionally, a parent dialog.
	cPictChoice(pic_num_t first, pic_num_t last, ePicType t, cDialog* parent = nullptr);
	/// Attach a handler to be called when the selected item changes.
	/// @param f A function that takes a reference to the dialog and the index of the newly selected item.
	void attachSelectHandler(std::function<void(cPictChoice&,int)> f);
	/// Reference the cDialog powering this choice dialog, perhaps to customize details of it.
	/// @return A pointer to the dialog.
	cDialog* operator->();
	/// Show the dialog.
	/// @param cur_sel The index of the icon that should be initially selected when the dialog is shown.
	/// @return false if the user clicked Cancel, true otherwise.
	bool show(size_t cur_sel);
	/// Get the chosen icon.
	/// @return The number of the chosen icon.
	pic_num_t getPicChosen();
	/// Get the chosen icon.
	/// @return The type of the chosen icon.
	ePicType getPicChosenType();
	/// Get the index of the selected icon in the original list.
	/// @return The index
	size_t getSelected();
};
#endif

/// Shows a simple error dialog for an unrecoverable condition.
/// @param str1 The first string in the error dialog.
/// @param str2 The second string in the error dialog.
/// @param parent Optionally, a parent dialog.
void showFatalError(std::string str1, std::string str2, cDialog* parent = nullptr);
/// Shows a simple error dialog for an unrecoverable condition.
/// @param str1 The string in the error dialog.
/// @param parent Optionally, a parent dialog.
void showFatalError(std::string str1, cDialog* parent = nullptr);

/// Shows a simple error dialog for a recoverable condition.
/// @param str1 The first string in the error dialog.
/// @param str2 The second string in the error dialog.
/// @param parent Optionally, a parent dialog.
void showError(std::string str1, std::string str2, cDialog* parent = nullptr);
/// Shows a simple error dialog for a recoverable condition.
/// @param str1 The string in the error dialog.
/// @param parent Optionally, a parent dialog.
void showError(std::string str1, cDialog* parent = nullptr);

/// Shows a simple warning message dialog.
/// @param str1 The first string in the warning dialog.
/// @param str2 The second string in the warning dialog.
/// @param parent Optionally, a parent dialog.
void showWarning(std::string str1, std::string str2, cDialog* parent = nullptr);
/// Shows a simple warning message dialog.
/// @param str1 The string in the warning dialog.
/// @param parent Optionally, a parent dialog.
void showWarning(std::string str1, cDialog* parent = nullptr);
