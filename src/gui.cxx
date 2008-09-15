/*
---       `funlab' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski         ---

	gui.c.gtkmm - this file is integral part of `funlab' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You can not use any part of sources of this software.
  2. You can not redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you can not demand any fees
     for this software.
     You can not even demand cost of the carrier (CD for example).
  5. You can not include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <gtkmm.h>
#include <libglademm/xml.h>
#include <cairomm/context.h>
#include <iostream>
#include <libintl.h>

#include <yaal/yaal.h>
M_VCSID( "$Id: "__ID__" $" )
#include "gui.h"

#include "version.h"
#include "setup.h"
#include "hdetachedrenderer.h"
#include "hembeddedrenderer.h"
#include "events.h"
#include "hfunlab.h"

using namespace std;
using namespace yaal::hcore;

namespace funlab
{

class HWindowMain : public Gtk::Window, public HKeyboardEventListener
	{
	class HLocker
		{
		bool& f_rbLock;
		explicit HLocker( bool& a_rbLock ) : f_rbLock( a_rbLock ) { f_rbLock = true; }
		virtual ~HLocker( void ) { f_rbLock = false; }
		friend class HWindowMain;
		};
protected:
	/*{*/
	bool f_bLock;
	Glib::RefPtr<Gtk::ListStore> f_oFormulasListModel;
	Gtk::TreeModel::ColumnRecord f_oFormulasListColumns;
	Gtk::TreeModelColumn<Glib::ustring> f_oFormulasListFormulaColumn;
	Gtk::TreeView* f_poFormulasListView;
	Glib::Dispatcher f_oDispatcher;
	bool f_bDetachedRendererActive;
	HEmbeddedRenderer* f_poEmbeddedRenderer;
	HDetachedRenderer f_oDetachedRenderer;
	/*}*/
public:
	/*{*/
	HWindowMain( BaseObjectType*, Glib::RefPtr<Gnome::Glade::Xml> const& );
	virtual ~HWindowMain( void );

	/*}*/
protected:
	/*{*/
	void shutdown_renderer( void );
	void on_new( void );
	void on_open( void );
	void on_save_as( void );
	void on_quit( void );
	void on_about( void );
	void on_add( void );
	void on_remove( void );
	void on_sel_changed( void );
	bool on_key_press( GdkEventKey* );
	virtual void do_on_event( HKeyboardEvent* );
	void show_error_message( char const* const, char const* const, int );
	void set_font_all( Pango::FontDescription const&, Gtk::Widget* );
	void open( HString const& );
	void save( HString const& );

	/*}*/
	};

HWindowMain::HWindowMain( BaseObjectType* a_poBaseObject,
	Glib::RefPtr<Gnome::Glade::Xml> const& a_roResources ) : Gtk::Window( a_poBaseObject ),
	f_bLock( false ), f_poFormulasListView( NULL ),
	f_bDetachedRendererActive( false ), f_oDetachedRenderer( this )
	{
	M_PROLOG
	Gtk::ToolButton* l_poToolButton = NULL;
	Gtk::MenuItem* l_poMenuItem = NULL;

	HRendererEngineInterface::ptr_t dre( new HFunlab( &f_oDetachedRenderer ) );
	f_oDetachedRenderer.set_engine( dre );

	/* FORMULAS LIST */
	a_roResources->get_widget( "TREE_FORMULAS", f_poFormulasListView );
	f_oFormulasListColumns.add( f_oFormulasListFormulaColumn );
	f_oFormulasListModel = Gtk::ListStore::create( f_oFormulasListColumns );
	f_poFormulasListView->set_model( f_oFormulasListModel );
	f_poFormulasListView->append_column_editable( _( "Formulas" ), f_oFormulasListFormulaColumn );
	Glib::RefPtr<Gtk::TreeSelection> l_oSelection = f_poFormulasListView->get_selection();
	l_oSelection->set_mode( Gtk::SELECTION_SINGLE );
	l_oSelection->signal_changed().connect( sigc::mem_fun( *this, &HWindowMain::on_sel_changed ) );
	f_poFormulasListView->signal_key_press_event().connect( sigc::mem_fun( *this, &HWindowMain::on_key_press ), false );
	f_poFormulasListView->grab_focus();
	
	f_oDispatcher.connect( sigc::mem_fun( *this, &HWindowMain::shutdown_renderer ) );

	a_roResources->get_widget_derived( "RENDERER", f_poEmbeddedRenderer );
	HRendererEngineInterface::ptr_t ere( new HFunlab( f_poEmbeddedRenderer ) );
	f_poEmbeddedRenderer->set_engine( ere );

	/* NEW */
	a_roResources->get_widget( "ID_TOOLBAR_NEW", l_poToolButton );
	l_poToolButton->signal_clicked().connect( sigc::mem_fun( *this, &HWindowMain::on_new ) );
	a_roResources->get_widget( "ID_MENU_NEW", l_poMenuItem );
	l_poMenuItem->signal_activate().connect( sigc::mem_fun( *this, &HWindowMain::on_new ) );
	/* OPEN */
	a_roResources->get_widget( "ID_TOOLBAR_OPEN", l_poToolButton );
	l_poToolButton->signal_clicked().connect( sigc::mem_fun( *this, &HWindowMain::on_open ) );
	a_roResources->get_widget( "ID_MENU_OPEN", l_poMenuItem );
	l_poMenuItem->signal_activate().connect( sigc::mem_fun( *this, &HWindowMain::on_open ) );

	/* SAVE_AS */
	a_roResources->get_widget( "ID_TOOLBAR_SAVE_AS", l_poToolButton );
	l_poToolButton->signal_clicked().connect( sigc::mem_fun( *this, &HWindowMain::on_save_as ) );
	a_roResources->get_widget( "ID_MENU_SAVE_AS", l_poMenuItem );
	l_poMenuItem->signal_activate().connect( sigc::mem_fun( *this, &HWindowMain::on_save_as ) );

	/* QUIT */
	a_roResources->get_widget( "ID_TOOLBAR_QUIT", l_poToolButton );
	l_poToolButton->signal_clicked().connect( sigc::mem_fun( *this, &HWindowMain::on_quit ) );
	a_roResources->get_widget( "ID_MENU_QUIT", l_poMenuItem );
	l_poMenuItem->signal_activate().connect( sigc::mem_fun( *this, &HWindowMain::on_quit ) );

	/* ABOUT */
	a_roResources->get_widget( "ID_TOOLBAR_ABOUT", l_poToolButton );
	l_poToolButton->signal_clicked().connect( sigc::mem_fun( *this, &HWindowMain::on_about ) );
	a_roResources->get_widget( "ID_MENU_ABOUT", l_poMenuItem );
	l_poMenuItem->signal_activate().connect( sigc::mem_fun( *this, &HWindowMain::on_about ) );

	/* ADD */
	a_roResources->get_widget( "ID_TOOLBAR_ADD", l_poToolButton );
	l_poToolButton->signal_clicked().connect( sigc::mem_fun( *this, &HWindowMain::on_add ) );
/*
   a_roResources->get_widget ( "ID_MENU_ADD", l_poMenuItem );
   l_poMenuItem->signal_activate().connect ( sigc::mem_fun ( * this, & HWindowMain::on_add ) );
 */

	/* REMOVE */
	a_roResources->get_widget( "ID_TOOLBAR_REMOVE", l_poToolButton );
	l_poToolButton->signal_clicked().connect( sigc::mem_fun( *this, &HWindowMain::on_remove ) );
/*
   a_roResources->get_widget ( "ID_MENU_REMOVE", l_poMenuItem );
   l_poMenuItem->signal_activate().connect ( sigc::mem_fun ( * this, & HWindowMain::on_remove ) );
 */
	return;
	M_EPILOG
	}

HWindowMain::~HWindowMain( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

void HWindowMain::on_new( void )
	{
	M_PROLOG
	HLocker l_oLock( f_bLock );

	f_oFormulasListModel->clear();
	return;
	M_EPILOG
	}

void HWindowMain::on_open( void )
	{
	M_PROLOG
	HLocker l_oLock( f_bLock );
	Gtk::FileFilter l_oFileFilter;
	Gtk::FileChooserDialog l_oFileOpenDialog( *this,
	                                          _( "Select formulas file to open ..." ), Gtk::FILE_CHOOSER_ACTION_OPEN );

	l_oFileOpenDialog.set_local_only( true );
	l_oFileOpenDialog.set_select_multiple( false );
	l_oFileFilter.set_name( _( "Function formulas." ) );
	l_oFileFilter.add_pattern( "*.fun" );
	l_oFileOpenDialog.add_filter( l_oFileFilter );
	l_oFileOpenDialog.add_button( Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL );
	l_oFileOpenDialog.add_button( Gtk::Stock::OPEN, Gtk::RESPONSE_OK );
	if ( l_oFileOpenDialog.run() == Gtk::RESPONSE_OK )
		open( l_oFileOpenDialog.get_filename().c_str() );

	return;
	M_EPILOG
	}

void HWindowMain::open( HString const& a_oPath )
	{
	M_PROLOG
	if ( a_oPath.is_empty() )
		M_THROW( _( "Empty path." ), errno );

	int l_iIndex = 0;
	HString l_oLine;
	HFile l_oFile;
	Gtk::TreeModel::Row l_oRow;
	l_oFile.open( a_oPath );
	if ( !!l_oFile )
		{
		f_oFormulasListModel->clear();
		while ( l_oFile.read_line( l_oLine, HFile::READ::D_STRIP_NEWLINES ) >= 0 )
			{
			l_oRow = *( f_oFormulasListModel->append() );
			l_oRow[ f_oFormulasListFormulaColumn ] = l_oLine.raw();
			l_iIndex ++;
			}
		l_oFile.close();
		}
	f_poFormulasListView->grab_focus();
	Glib::RefPtr<Gtk::TreeSelection> l_oSelection = f_poFormulasListView->get_selection();
	if ( l_iIndex )
		{
		Gtk::TreeModel::Children l_oRows = f_oFormulasListModel->children();
		Gtk::TreeIter l_oIter = l_oRows.begin();
		f_poFormulasListView->set_cursor ( f_oFormulasListModel->get_path ( l_oIter ) );
		}
	return;
	M_EPILOG
	}

void HWindowMain::on_save_as( void )
	{
	M_PROLOG
	HLocker l_oLock( f_bLock );
	Gtk::FileFilter l_oFileFilter;
	Gtk::FileChooserDialog l_oFileOpenDialog( *this,
	                                          _( "Enter file name to save Your formulas ..." ), Gtk::FILE_CHOOSER_ACTION_SAVE );

	l_oFileOpenDialog.set_local_only( true );
	l_oFileOpenDialog.set_select_multiple( false );
	l_oFileFilter.set_name( _( "Function formulas." ) );
	l_oFileFilter.add_pattern( "*.fun" );
	l_oFileOpenDialog.add_filter( l_oFileFilter );
	l_oFileOpenDialog.add_button( Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL );
	l_oFileOpenDialog.add_button( Gtk::Stock::SAVE, Gtk::RESPONSE_OK );
	if ( l_oFileOpenDialog.run() == Gtk::RESPONSE_OK )
		save( l_oFileOpenDialog.get_filename().c_str() );

	return;
	M_EPILOG
	}

void HWindowMain::save( HString const& a_oPath )
	{
	M_PROLOG
	if ( a_oPath.is_empty() )
		M_THROW( _( "Empty path." ), errno );

	HFile l_oFile( HFile::OPEN::D_WRITING );
	l_oFile.open( a_oPath );
	if ( !!l_oFile )
		{
		Gtk::TreeModel::Children l_oRows = f_oFormulasListModel->children();
		for ( Gtk::TreeIter l_oIter = l_oRows.begin(); l_oIter != l_oRows.end(); ++ l_oIter )
			l_oFile << l_oIter->get_value( f_oFormulasListFormulaColumn ).c_str() << endl;

		l_oFile.close();
		}
	return;
	M_EPILOG
	}

void HWindowMain::on_quit( void )
	{
	M_PROLOG
	HLocker l_oLock( f_bLock );

	Gtk::Main::quit();
	return;
	M_EPILOG
	}

void HWindowMain::on_about( void )
	{
	M_PROLOG
	HLocker l_oLock( f_bLock );
	HString l_oMsg;
	l_oMsg.format( _( "The funlab program, very fancy GUI application that\n"
			"can be used for rendering function surfaces.\n"
			"Welcome to funlab %s" ), VER );
	Gtk::MessageDialog l_oMessageAbout( *this, l_oMsg.raw() );
	l_oMessageAbout.set_title( _( "About funlab" ) );
	l_oMessageAbout.run();
	return;
	M_EPILOG
	}

void HWindowMain::on_add( void )
	{
	M_PROLOG
	HLocker l_oLock( f_bLock );
	Gtk::TreeIter l_oIter = f_oFormulasListModel->append();
	f_poFormulasListView->set_cursor( f_oFormulasListModel->get_path( l_oIter ) );
	f_poFormulasListView->grab_focus();
	return;
	M_EPILOG
	}

void HWindowMain::on_remove( void )
	{
	M_PROLOG
	HLocker l_oLock( f_bLock );

	Glib::RefPtr<Gtk::TreeSelection> l_oSelection = f_poFormulasListView->get_selection();
	Gtk::TreeIter l_oIter = l_oSelection->get_selected();
	if ( l_oIter )
		{
		Gtk::TreeModel::Children l_oRows = f_oFormulasListModel->children();
		Gtk::TreeIter l_oIterNew = l_oIter;
		++ l_oIterNew;
		if ( l_oIterNew == l_oRows.end() )
			{
			if ( l_oIter != l_oRows.begin() )
				{
				l_oIterNew = l_oIter;
				-- l_oIter;
				}
			}

		f_oFormulasListModel->erase( l_oIter );
		if ( l_oIterNew )
			f_poFormulasListView->set_cursor( f_oFormulasListModel->get_path( l_oIterNew ) );

		f_poFormulasListView->grab_focus();
		}
	return;
	M_EPILOG
	}

void HWindowMain::on_sel_changed( void )
	{
	M_PROLOG
	Glib::RefPtr<Gtk::TreeSelection> l_oSelection = f_poFormulasListView->get_selection();
	Gtk::TreeIter l_oIter = l_oSelection->get_selected();
	if ( ! f_bLock && f_bDetachedRendererActive && l_oIter )
		dynamic_cast<HFunlab*>( &(*f_oDetachedRenderer.get_engine()) )->push_formula( l_oIter->get_value( f_oFormulasListFormulaColumn ).c_str() );

	return;
	M_EPILOG
	}

bool HWindowMain::on_key_press( GdkEventKey* a_poEventKey )
	{
	M_PROLOG
	switch ( a_poEventKey->keyval )
		{
		case ( GDK_Delete ):
			on_remove();
		break;
		case ( GDK_Insert ):
			on_add();
		break;
		case ( ' ' ):
			{
			Glib::RefPtr<Gtk::TreeSelection> l_oSelection = f_poFormulasListView->get_selection();
			Gtk::TreeIter l_oIter = l_oSelection->get_selected();
			if ( l_oIter )
				{
				HString l_oFormula = l_oIter->get_value ( f_oFormulasListFormulaColumn ).c_str();
				if ( dynamic_cast<HFunlab*>( &(*f_oDetachedRenderer.get_engine() ) )->push_formula( l_oFormula ) )
					show_error_message( l_oFormula.raw(), f_oDetachedRenderer.error(), f_oDetachedRenderer.error_position() );
				f_bDetachedRendererActive = true;
				}
			}
		break;
		default:
		break;
		}
	return ( false );
	M_EPILOG
	}

void HWindowMain::shutdown_renderer( void )
	{
	cout << __PRETTY_FUNCTION__ << endl;
	f_oDetachedRenderer.shutdown();
	}

void HWindowMain::do_on_event( HKeyboardEvent* e )
	{
	cout << __PRETTY_FUNCTION__ << endl;
	if ( e->get_code() == 'q' )
		f_oDispatcher();
	return;
	}

void HWindowMain::show_error_message( char const* const a_pcFormula,
	char const* const a_pcMessage, int a_iPosition )
	{
	M_PROLOG
	int l_iCtr = 0;
	HString l_oError, l_oArrow;
	for ( l_iCtr = 0; l_iCtr < a_iPosition; l_iCtr ++ )
		l_oArrow += '-';
	l_oArrow += 'v';
	l_oError.format( "<b>%s at this place:<tt>\n\n%s\n%s</tt></b>",
		a_pcMessage, l_oArrow.raw(),
		a_pcFormula );
	Gtk::MessageDialog l_oInfo( *this,
			l_oError.raw(), true );
	l_oInfo.set_title( _( "Formula syntax error ..." ) );
	Pango::FontDescription l_oFontDesc( "Sans Bold 14" );
	set_font_all( l_oFontDesc, &l_oInfo );
	l_oInfo.run();
	return;
	M_EPILOG
	}

void HWindowMain::set_font_all( Pango::FontDescription const& a_oFontDesc,
	Gtk::Widget* a_poWidget )
	{
	M_PROLOG
	if ( !a_poWidget )
		return;
	a_poWidget->modify_font( a_oFontDesc );
	Gtk::Dialog* l_poDialog = dynamic_cast<Gtk::Dialog*>( a_poWidget );
	Gtk::Box* l_poBox = dynamic_cast<Gtk::Box*>( a_poWidget );
	Gtk::Window* l_poWindow = dynamic_cast<Gtk::Window*>( a_poWidget );
	Gtk::Label* l_poLabel = dynamic_cast<Gtk::Label*>( a_poWidget );
	if ( l_poLabel )
		l_poLabel->set_line_wrap( false );
	if ( l_poDialog )
		{
		set_font_all( a_oFontDesc, l_poDialog->get_vbox() );
		set_font_all( a_oFontDesc, l_poDialog->get_action_area() );
		}
	else if ( l_poWindow )
		{
		Glib::ListHandle<Widget*> l_oChildren = l_poWindow->get_children();
		for ( Glib::ListHandle<Widget*>::iterator l_oChildIterator = l_oChildren.begin();
		      l_oChildIterator != l_oChildren.end(); ++ l_oChildIterator )
			set_font_all( a_oFontDesc, *l_oChildIterator );
		}
	else if ( l_poBox )
		{
		Gtk::Box_Helpers::BoxList& l_oBoxList = l_poBox->children();
		for ( Gtk::Box_Helpers::BoxList::iterator l_oBoxIterator = l_oBoxList.begin();
		      l_oBoxIterator != l_oBoxList.end(); ++ l_oBoxIterator )
			set_font_all( a_oFontDesc, l_oBoxIterator->get_widget() );

		Glib::ListHandle<Widget*> l_oChildren = l_poBox->get_children();
		for ( Glib::ListHandle<Widget*>::iterator l_oChildIterator = l_oChildren.begin();
		      l_oChildIterator != l_oChildren.end(); ++ l_oChildIterator )
			set_font_all( a_oFontDesc, *l_oChildIterator );
		}

//	Gtk::MessageDialog o ( * this, a_poWidget->get_name() );
//	o.run();
	return;
	M_EPILOG
	}

int gui_start( int a_iArgc, char* a_ppcArgv[] )
	{
	M_PROLOG
	try
		{
		Gtk::Main l_oGUI( a_iArgc, a_ppcArgv );
		Glib::RefPtr<Gnome::Glade::Xml> l_oResources = Gnome::Glade::Xml::create(
			setup.f_oResourcePath.raw() );
		HWindowMain* l_poWindowMain = NULL;
		l_oResources->get_widget_derived( "WINDOW_MAIN", l_poWindowMain );
		l_oGUI.run( *l_poWindowMain );
		}
	catch ( Glib::Exception& e )
		{
		cerr << "Glib::Exception: " << e.what() << endl;
		return ( -1 );
		}
	catch ( std::exception& e )
		{
		cerr << "std::exception: " << e.what() << endl;
		return ( -1 );
		}
	return ( 0 );
	M_EPILOG
	}

}

/* vim: set ft=cpp : */
