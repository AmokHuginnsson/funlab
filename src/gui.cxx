/*
---       `funlab' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski         ---

	gui.c.gtkmm - this file is integral part of `funlab' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <gtkmm.h>
#include <cairomm/context.h>
#include <libintl.h>

#include <yaal/yaal.hxx>
M_VCSID( "$Id: "__ID__" $" )
#include "gui.hxx"

#include "setup.hxx"
#include "hdetachedrenderer.hxx"
#include "hembeddedrenderer.hxx"
#include "events.hxx"
#include "hfunlab.hxx"
#include "oplotdesc.hxx"

using namespace yaal::hcore;
using namespace yaal::tools;

namespace funlab {

class HWindowMain : public Gtk::Window, public HKeyboardEventListener {
	class HLocker {
		bool& _lock;
		explicit HLocker( bool& lock_ ) : _lock( lock_ ) { _lock = true; }
		virtual ~HLocker( void ) { _lock = false; }
		friend class HWindowMain;
	};
protected:
	bool _lock;
	Glib::RefPtr<Gtk::ListStore> _formulasListModel;
	Gtk::TreeModel::ColumnRecord _formulasListColumns;
	Gtk::TreeModelColumn<OPlotDesc> _formulasListFormulaColumn;
	Gtk::TreeView* _formulasListView;
	Gtk::Scale* _density;
	Gtk::CheckButton* f_po3D;
	Gtk::CheckButton* _multiFormula;
	Gtk::CheckButton* _showAxis;
	Gtk::CheckButton* _stereo;
	Gtk::SpinButton* _domainLowerBound;
	Gtk::SpinButton* _domainUpperBound;
	Gtk::SpinButton* _rangeLowerBound;
	Gtk::SpinButton* _rangeUpperBound;
	Gtk::Entry* _formula;
	Glib::Dispatcher _dispatcher;
	bool _detachedRendererActive;
	HEmbeddedRenderer* _embeddedRenderer;
	HDetachedRenderer::ptr_t _detachedRenderer;
public:
	HWindowMain( BaseObjectType*, Glib::RefPtr<Gtk::Builder> const& );
	virtual ~HWindowMain( void );
protected:
	void shutdown_renderer( void );
	void on_new( void );
	void on_open( void );
	void on_save_as( void );
	void on_quit( void );
	void on_about( void );
	void on_add( void );
	void on_remove( void );
	void on_sel_changed( void );
	void on_density_changed( void );
	void on_show_axis_changed( void );
	void on_stereo_changed( void );
	void on_3d_changed( void );
	void on_multi_changed( void );
	void on_domain_lower_bound_changed( void );
	void on_domain_upper_bound_changed( void );
	void on_range_lower_bound_changed( void );
	void on_range_upper_bound_changed( void );
	void on_formula_changed( void );
	void on_plot_data_changed( void );
	void update_drawing( bool = true );
	bool on_key_press( GdkEventKey* );
	virtual void do_on_event( HKeyboardEvent const* );
	void show_error_message( char const* const, char const* const, int );
	void set_font_all( Pango::FontDescription const&, Gtk::Widget* );
	void selected_row_callback( Gtk::TreeModel::iterator const& );
	static void get_value_for_cell( Gtk::CellRenderer*, Gtk::TreeModel::iterator const&, Gtk::TreeModelColumn<OPlotDesc> const& );
	HFunlab* funlab( void );
	void open( HString const& );
	void save( HString const& );
};

HWindowMain::HWindowMain( BaseObjectType* baseObject_,
	Glib::RefPtr<Gtk::Builder> const& resources_ ) : Gtk::Window( baseObject_ ),
	_lock( false ), _formulasListView( NULL ), _density( NULL ),
	f_po3D( NULL ), _multiFormula( NULL ), _showAxis( NULL ),
	_domainLowerBound( NULL ), _domainUpperBound( NULL ),
	_rangeLowerBound( NULL ), _rangeUpperBound( NULL ),
	_formula( NULL ),
	_detachedRendererActive( false ), _detachedRenderer() {
	M_PROLOG
	Gtk::ToolButton* toolButton = NULL;
	Gtk::MenuItem* menuItem = NULL;

	resources_->get_widget_derived( "RENDERER", _embeddedRenderer );
	HRendererEngineInterface::ptr_t ere( make_pointer<HFunlab>( _embeddedRenderer ) );
	_embeddedRenderer->set_engine( ere );

	/* Formulas List */
	resources_->get_widget( "TREE_FORMULAS", _formulasListView );
	_formulasListColumns.add( _formulasListFormulaColumn );
	_formulasListModel = Gtk::ListStore::create( _formulasListColumns );
	_formulasListView->set_model( _formulasListModel );

	/* Very nontrivial column appending! */

/*
 * Use a CellRendererText:
 * We don't use TreeView::Column::append_column(model_column) to generate an appropriate CellRenderer,
 * because that uses set_renderer(), which renders the model value using the automatic glib "transformations"
 * (number-string conversions). As well as being unnecessary here, those automatic conversions can't handle all numeric types.
 *
 * Some compilers don't like us to give the pointer to a template function directly to sigc::ptr_fun():
 *
 * Connect a cell_data callback, to show the number's text representation in the specified format:
 * We use sigc::bind<-1> twice here, instead of sigc::bind() once, because some compilers need the extra hint.
*/

	Gtk::TreeViewColumn* const col = Gtk::manage( new Gtk::TreeViewColumn( _( "Formulas" ) ) );
	Gtk::CellRenderer* pCellRenderer = manage( new Gtk::CellRendererText() );
  col->pack_start( *pCellRenderer );
  typedef void ( *get_value_for_cell_t )( Gtk::CellRenderer*, Gtk::TreeModel::iterator const&, Gtk::TreeModelColumn<OPlotDesc> const& );
  get_value_for_cell_t value_getter = &HWindowMain::get_value_for_cell;
  Gtk::TreeViewColumn::SlotCellData slot = sigc::bind<-1>(
			sigc::ptr_fun( value_getter ),
			_formulasListFormulaColumn );
	col->set_cell_data_func( *pCellRenderer, slot );
	_formulasListView->append_column( *col );

	/* Rest of this stuff is plain and simple. */

	Glib::RefPtr<Gtk::TreeSelection> selection = _formulasListView->get_selection();
	selection->set_mode( Gtk::SELECTION_SINGLE );
	selection->signal_changed().connect( sigc::mem_fun( *this, &HWindowMain::on_sel_changed ) );
	_formulasListView->signal_key_press_event().connect( sigc::mem_fun( *this, &HWindowMain::on_key_press ), false );
	_formulasListView->grab_focus();

	resources_->get_widget( "DENSITY", _density );
	_density->set_value( setup._density );
	_density->signal_value_changed().connect( sigc::mem_fun( *this, &HWindowMain::on_density_changed ) );

	resources_->get_widget( "AXIS", _showAxis );
	_showAxis->set_active( setup._showAxis );
	_showAxis->signal_toggled().connect( sigc::mem_fun( *this, &HWindowMain::on_show_axis_changed ) );

	resources_->get_widget( "STEREO", _stereo );
	_stereo->set_active( setup._stereo );
	_stereo->signal_toggled().connect( sigc::mem_fun( *this, &HWindowMain::on_stereo_changed ) );

	resources_->get_widget( "MULTI", _multiFormula );
	_multiFormula->set_active( setup._multiFormula );
	_multiFormula->signal_toggled().connect( sigc::mem_fun( *this, &HWindowMain::on_multi_changed ) );

	resources_->get_widget( "DOMAIN_LOWER_BOUND", _domainLowerBound );
	_domainLowerBound->set_value( static_cast<double>( setup._domainLowerBound ) );
	_domainLowerBound->signal_value_changed().connect( sigc::mem_fun( *this, &HWindowMain::on_domain_lower_bound_changed ) );

	resources_->get_widget( "DOMAIN_UPPER_BOUND", _domainUpperBound );
	_domainUpperBound->set_value( static_cast<double>( setup._domainUpperBound ) );
	_domainUpperBound->signal_value_changed().connect( sigc::mem_fun( *this, &HWindowMain::on_domain_upper_bound_changed ) );

	resources_->get_widget( "RANGE_LOWER_BOUND", _rangeLowerBound );
	_rangeLowerBound->set_value( static_cast<double>( setup._rangeLowerBound ) );
	_rangeLowerBound->signal_value_changed().connect( sigc::mem_fun( *this, &HWindowMain::on_range_lower_bound_changed ) );

	resources_->get_widget( "RANGE_UPPER_BOUND", _rangeUpperBound );
	_rangeUpperBound->set_value( static_cast<double>( setup._rangeUpperBound ) );
	_rangeUpperBound->signal_value_changed().connect( sigc::mem_fun( *this, &HWindowMain::on_range_upper_bound_changed ) );

	resources_->get_widget( "FORMULA", _formula );
	_formula->signal_changed().connect( sigc::mem_fun( *this, &HWindowMain::on_formula_changed ) );

	resources_->get_widget( "MODE_3D", f_po3D );
	f_po3D->set_active( setup.f_b3D );
	f_po3D->signal_toggled().connect( sigc::mem_fun( *this, &HWindowMain::on_3d_changed ) );

	_dispatcher.connect( sigc::mem_fun( *this, &HWindowMain::shutdown_renderer ) );

	/* NEW */
	resources_->get_widget( "ID_TOOLBAR_NEW", toolButton );
	toolButton->signal_clicked().connect( sigc::mem_fun( *this, &HWindowMain::on_new ) );
	resources_->get_widget( "ID_MENU_NEW", menuItem );
	menuItem->signal_activate().connect( sigc::mem_fun( *this, &HWindowMain::on_new ) );
	/* OPEN */
	resources_->get_widget( "ID_TOOLBAR_OPEN", toolButton );
	toolButton->signal_clicked().connect( sigc::mem_fun( *this, &HWindowMain::on_open ) );
	resources_->get_widget( "ID_MENU_OPEN", menuItem );
	menuItem->signal_activate().connect( sigc::mem_fun( *this, &HWindowMain::on_open ) );

	/* SAVE_AS */
	resources_->get_widget( "ID_TOOLBAR_SAVE_AS", toolButton );
	toolButton->signal_clicked().connect( sigc::mem_fun( *this, &HWindowMain::on_save_as ) );
	resources_->get_widget( "ID_MENU_SAVE_AS", menuItem );
	menuItem->signal_activate().connect( sigc::mem_fun( *this, &HWindowMain::on_save_as ) );

	/* QUIT */
	resources_->get_widget( "ID_TOOLBAR_QUIT", toolButton );
	toolButton->signal_clicked().connect( sigc::mem_fun( *this, &HWindowMain::on_quit ) );
	resources_->get_widget( "ID_MENU_QUIT", menuItem );
	menuItem->signal_activate().connect( sigc::mem_fun( *this, &HWindowMain::on_quit ) );

	/* ABOUT */
	resources_->get_widget( "ID_TOOLBAR_ABOUT", toolButton );
	toolButton->signal_clicked().connect( sigc::mem_fun( *this, &HWindowMain::on_about ) );
	resources_->get_widget( "ID_MENU_ABOUT", menuItem );
	menuItem->signal_activate().connect( sigc::mem_fun( *this, &HWindowMain::on_about ) );

	/* ADD */
	resources_->get_widget( "ID_TOOLBAR_ADD", toolButton );
	toolButton->signal_clicked().connect( sigc::mem_fun( *this, &HWindowMain::on_add ) );
/*
   resources_->get_widget ( "ID_MENU_ADD", menuItem );
   menuItem->signal_activate().connect ( sigc::mem_fun ( * this, & HWindowMain::on_add ) );
 */

	/* REMOVE */
	resources_->get_widget( "ID_TOOLBAR_REMOVE", toolButton );
	toolButton->signal_clicked().connect( sigc::mem_fun( *this, &HWindowMain::on_remove ) );
/*
   resources_->get_widget ( "ID_MENU_REMOVE", menuItem );
   menuItem->signal_activate().connect ( sigc::mem_fun ( * this, & HWindowMain::on_remove ) );
 */
	return;
	M_EPILOG
}

HWindowMain::~HWindowMain( void ) {
	M_PROLOG
	return;
	M_EPILOG
}

void HWindowMain::on_new( void ) {
	M_PROLOG
	HLocker lock( _lock );

	_formulasListModel->clear();
	return;
	M_EPILOG
}

void HWindowMain::on_open( void ) {
	M_PROLOG
	HLocker lock( _lock );
	Gtk::FileFilter fileFilter;
	Gtk::FileChooserDialog fileOpenDialog( *this, _( "Select formulas file to open ..." ), Gtk::FILE_CHOOSER_ACTION_OPEN );
	fileOpenDialog.set_local_only( true );
	fileOpenDialog.set_select_multiple( false );
	fileFilter.set_name( _( "Function formulas." ) );
	fileFilter.add_pattern( "*.fun" );
	fileOpenDialog.add_filter( fileFilter );
	fileOpenDialog.add_button( Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL );
	fileOpenDialog.add_button( Gtk::Stock::OPEN, Gtk::RESPONSE_OK );
	if ( fileOpenDialog.run() == Gtk::RESPONSE_OK )
		open( fileOpenDialog.get_filename().c_str() );

	return;
	M_EPILOG
}

void HWindowMain::open( HString const& path_ ) {
	M_PROLOG
	if ( path_.is_empty() )
		M_THROW( _( "Empty path." ), errno );

	int index = 0;
	HString line;
	Gtk::TreeModel::Row row;
	try {
		HFile file( path_, HFile::OPEN::READING );
		if ( !!file ) {
			_formulasListModel->clear();
			while ( file.read_line( line ) >= 0 ) {
				row = *( _formulasListModel->append() );
				row[ _formulasListFormulaColumn ] = plot_desc_from_string( line );
				index ++;
			}
		}
	} catch ( HException const& e ) {
		Gtk::MessageDialog info( *this, e.what(), true );
		info.set_title( _( "Error loading file ..." ) );
		Pango::FontDescription fontDesc( "Sans Bold 14" );
		set_font_all( fontDesc, &info );
		info.run();
	}
	if ( index ) {
		_formulasListView->grab_focus();
		Glib::RefPtr<Gtk::TreeSelection> selection = _formulasListView->get_selection();
		Gtk::TreeModel::Children rows = _formulasListModel->children();
		Gtk::TreeIter iter = rows.begin();
		_formulasListView->set_cursor ( _formulasListModel->get_path ( iter ) );
	}
	return;
	M_EPILOG
}

void HWindowMain::on_save_as( void ) {
	M_PROLOG
	HLocker lock( _lock );
	Gtk::FileFilter fileFilter;
	Gtk::FileChooserDialog fileOpenDialog( *this, _( "Enter file name to save Your formulas ..." ), Gtk::FILE_CHOOSER_ACTION_SAVE );
	fileOpenDialog.set_local_only( true );
	fileOpenDialog.set_select_multiple( false );
	fileFilter.set_name( _( "Function formulas." ) );
	fileFilter.add_pattern( "*.fun" );
	fileOpenDialog.add_filter( fileFilter );
	fileOpenDialog.add_button( Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL );
	fileOpenDialog.add_button( Gtk::Stock::SAVE, Gtk::RESPONSE_OK );
	if ( fileOpenDialog.run() == Gtk::RESPONSE_OK )
		save( fileOpenDialog.get_filename().c_str() );
	return;
	M_EPILOG
}

void HWindowMain::save( HString const& path_ ) {
	M_PROLOG
	if ( path_.is_empty() )
		M_THROW( _( "Empty path." ), errno );

	HFile file( path_, HFile::OPEN::WRITING );
	if ( !!file ) {
		Gtk::TreeModel::Children rows = _formulasListModel->children();
		for ( Gtk::TreeIter iter = rows.begin(); iter != rows.end(); ++ iter )
			file << iter->get_value( _formulasListFormulaColumn ) << endl;

		file.close();
	}
	return;
	M_EPILOG
}

void HWindowMain::on_quit( void ) {
	M_PROLOG
	HLocker lock( _lock );

	Gtk::Main::quit();
	return;
	M_EPILOG
}

void HWindowMain::on_about( void ) {
	M_PROLOG
	HLocker lock( _lock );
	HString msg;
	msg.format( _( "The funlab program, very fancy GUI application that\n"
			"can be used for rendering function surfaces.\n"
			"Welcome to funlab %s" ), PACKAGE_STRING );
	Gtk::MessageDialog messageAbout( *this, msg.raw() );
	messageAbout.set_title( _( "About funlab" ) );
	messageAbout.run();
	return;
	M_EPILOG
}

void HWindowMain::on_add( void ) {
	M_PROLOG
	HLocker lock( _lock );
	Gtk::TreeIter iter = _formulasListModel->append();
	_formulasListView->set_cursor( _formulasListModel->get_path( iter ) );
	_formulasListView->grab_focus();
	return;
	M_EPILOG
}

void HWindowMain::on_remove( void ) {
	M_PROLOG
	HLocker lock( _lock );

	Glib::RefPtr<Gtk::TreeSelection> selection = _formulasListView->get_selection();
	Gtk::TreeIter iter = selection->get_selected();
	if ( iter ) {
		Gtk::TreeModel::Children rows = _formulasListModel->children();
		Gtk::TreeIter iterNew = iter;
		++ iterNew;
		if ( iterNew == rows.end() ) {
			if ( iter != rows.begin() ) {
				iterNew = iter;
				-- iterNew;
			}
		}

		_formulasListModel->erase( iter );
		if ( iterNew )
			_formulasListView->set_cursor( _formulasListModel->get_path( iterNew ) );

		_formulasListView->grab_focus();
	}
	return;
	M_EPILOG
}

HFunlab* HWindowMain::funlab( void ) {
	M_PROLOG
	HFunlab* f = NULL;
	if ( ! _lock && _detachedRendererActive ) {
		HDetachedRenderer* dr = dynamic_cast<HDetachedRenderer*>( &*_detachedRenderer );
		if ( dr )
			f = dynamic_cast<HFunlab*>( &( *dr->get_engine() ) );
	} else {
		HEmbeddedRenderer* er = dynamic_cast<HEmbeddedRenderer*>( _embeddedRenderer );
		if ( er )
			f = dynamic_cast<HFunlab*>( &( *er->get_engine() ) );
	}
	return ( f );
	M_EPILOG
}

void HWindowMain::selected_row_callback( Gtk::TreeModel::iterator const& iter ) {
	M_PROLOG
	HFunlab* f = NULL;
	if ( iter && ( f = funlab() ) ) {
		f->push_formula( iter->get_value( _formulasListFormulaColumn ) );
		update_drawing( false );
	}
	M_EPILOG
}

void HWindowMain::on_sel_changed( void ) {
	M_PROLOG
	Glib::RefPtr<Gtk::TreeSelection> selection = _formulasListView->get_selection();
	HFunlab* f = funlab();
	if ( f ) {
		f->clear();
		if ( selection->get_mode() == Gtk::SELECTION_MULTIPLE )
			selection->selected_foreach_iter( sigc::mem_fun( *this, &HWindowMain::selected_row_callback ) );
		else {
			Gtk::TreeIter iter = selection->get_selected();
			if ( iter ) {
				OPlotDesc plot = iter->get_value( _formulasListFormulaColumn );
				_formula->set_text( plot._formula.raw() );
				f_po3D->set_active( plot._3d );
				_domainLowerBound->set_value( static_cast<double>( plot._domainLowerBound ) );
				_domainUpperBound->set_value( static_cast<double>( plot._domainUpperBound ) );
				_rangeLowerBound->set_value( static_cast<double>( plot._rangeLowerBound ) );
				_rangeUpperBound->set_value( static_cast<double>( plot._rangeUpperBound ) );
				f->push_formula( plot );
				update_drawing( false );
			}
		}
	}
	return;
	M_EPILOG
}

void HWindowMain::update_drawing( bool full ) {
	if ( ! _lock && _detachedRendererActive ) {
		HDetachedRenderer* dr = dynamic_cast<HDetachedRenderer*>( &*_detachedRenderer );
		M_ASSERT( dr );
	} else {
		HEmbeddedRenderer* er = dynamic_cast<HEmbeddedRenderer*>( _embeddedRenderer );
		M_ASSERT( er );
		er->invoke_refresh( full );
	}
}

bool HWindowMain::on_key_press( GdkEventKey* eventKey_ ) {
	M_PROLOG
	switch ( eventKey_->keyval ) {
		case ( GDK_Delete ):
			on_remove();
		break;
		case ( GDK_Insert ):
			on_add();
		break;
		case ( ' ' ): {
			Glib::RefPtr<Gtk::TreeSelection> selection = _formulasListView->get_selection();
			Gtk::TreeIter iter = selection->get_selected();
			if ( iter ) {
				OPlotDesc const& plot = iter->get_value( _formulasListFormulaColumn );
				_detachedRenderer = make_pointer<HDetachedRenderer>( this );
				HDetachedRenderer* dr( static_cast<HDetachedRenderer*>( _detachedRenderer.get() ) );
				HRendererEngineInterface::ptr_t dre( make_pointer<HFunlab>( &*_detachedRenderer ) );
				dr->set_engine( dre );
				HFunlab* f = dynamic_cast<HFunlab*>( &(*dr->get_engine() ) );
				if ( f && f->push_formula( plot ) )
					show_error_message( plot._formula.raw(), f->error(), f->error_position() );
				else
					dr->render_surface();
				_detachedRendererActive = true;
			}
		}
		break;
		default:
		break;
	}
	return ( false );
	M_EPILOG
}

void HWindowMain::shutdown_renderer( void ) {
	cout << __PRETTY_FUNCTION__ << endl;
	dynamic_cast<HDetachedRenderer*>( &*_detachedRenderer )->shutdown();
	_detachedRenderer = HDetachedRenderer::ptr_t();
	_detachedRendererActive = false;
}

void HWindowMain::do_on_event( HKeyboardEvent const* e ) {
	cout << __PRETTY_FUNCTION__ << endl;
	if ( e->get_code() == 'q' )
		_dispatcher();
	return;
}

void HWindowMain::show_error_message( char const* const formula_,
	char const* const message_, int position_ ) {
	M_PROLOG
	int ctr = 0;
	HString error, arrow;
	for ( ctr = 0; ctr < position_; ctr ++ )
		arrow += '-';
	arrow += 'v';
	error.format( "<b>%s at this place:<tt>\n\n%s\n%s</tt></b>",
		message_, arrow.raw(),
		formula_ );
	Gtk::MessageDialog info( *this,
			error.raw(), true );
	info.set_title( _( "Formula syntax error ..." ) );
	Pango::FontDescription fontDesc( "Sans Bold 14" );
	set_font_all( fontDesc, &info );
	info.run();
	return;
	M_EPILOG
}

void HWindowMain::set_font_all( Pango::FontDescription const& fontDesc_,
	Gtk::Widget* widget_ ) {
	M_PROLOG
	if ( !widget_ )
		return;
	widget_->modify_font( fontDesc_ );
	Gtk::Dialog* dialog = dynamic_cast<Gtk::Dialog*>( widget_ );
	Gtk::Box* box = dynamic_cast<Gtk::Box*>( widget_ );
	Gtk::Window* window = dynamic_cast<Gtk::Window*>( widget_ );
	Gtk::Label* label = dynamic_cast<Gtk::Label*>( widget_ );
	if ( label )
		label->set_line_wrap( false );
	if ( dialog ) {
		set_font_all( fontDesc_, dialog->get_vbox() );
		set_font_all( fontDesc_, dialog->get_action_area() );
	} else if ( window ) {
		Glib::ListHandle<Widget*> children = window->get_children();
		for ( Glib::ListHandle<Widget*>::iterator childIterator = children.begin();
		      childIterator != children.end(); ++ childIterator )
			set_font_all( fontDesc_, *childIterator );
	} else if ( box ) {
		Gtk::Box_Helpers::BoxList& boxList = box->children();
		for ( Gtk::Box_Helpers::BoxList::iterator boxIterator = boxList.begin();
		      boxIterator != boxList.end(); ++ boxIterator )
			set_font_all( fontDesc_, boxIterator->get_widget() );

		Glib::ListHandle<Widget*> children = box->get_children();
		for ( Glib::ListHandle<Widget*>::iterator childIterator = children.begin();
		      childIterator != children.end(); ++ childIterator )
			set_font_all( fontDesc_, *childIterator );
	}

//	Gtk::MessageDialog o ( * this, widget_->get_name() );
//	o.run();
	return;
	M_EPILOG
}

void HWindowMain::on_density_changed( void ) {
	setup._density = static_cast<int>( _density->get_value() );
	update_drawing();
}

void HWindowMain::on_show_axis_changed( void ) {
	setup._showAxis = _showAxis->get_active();
	update_drawing();
}

void HWindowMain::on_stereo_changed( void ) {
	setup._stereo = _stereo->get_active();
	update_drawing();
}

void HWindowMain::on_3d_changed( void ) {
	on_plot_data_changed();
}

void HWindowMain::on_multi_changed( void ) {
	setup._multiFormula = _multiFormula->get_active();
	Glib::RefPtr<Gtk::TreeSelection> selection = _formulasListView->get_selection();
	selection->set_mode( setup._multiFormula ? Gtk::SELECTION_MULTIPLE : Gtk::SELECTION_SINGLE );
	update_drawing();
}

void HWindowMain::on_plot_data_changed( void ) {
	OPlotDesc plot;
	plot._3d = f_po3D->get_active();
	plot._domainLowerBound = _domainLowerBound->get_value();
	plot._domainUpperBound = _domainUpperBound->get_value();
	plot._rangeLowerBound = _rangeLowerBound->get_value();
	plot._rangeUpperBound = _rangeUpperBound->get_value();
	plot._formula = _formula->get_text().c_str();
	Glib::RefPtr<Gtk::TreeSelection> selection = _formulasListView->get_selection();
	if ( selection->get_mode() != Gtk::SELECTION_MULTIPLE ) {
		Gtk::TreeIter iter = selection->get_selected();
		if ( iter )
			iter->set_value( _formulasListFormulaColumn, plot );
	}
	HFunlab* f = funlab();
	if ( f ) {
		f->clear();
		f->push_formula( plot );
		update_drawing();
	}
	return;
}

void HWindowMain::on_formula_changed( void ) {
	HExpression e;
	Glib::ustring s( _formula->get_text() );
	try {
		e.compile( s.c_str() );
		on_plot_data_changed();
	} catch ( HExpressionException const& ex ) {
/*		_formula->select_region( e.get_error_token(), static_cast<int>( s.length() - 1 ) ); */
	}
	return;
}

void HWindowMain::on_domain_lower_bound_changed( void ) {
	double nval = _domainLowerBound->get_value();
	if ( nval < setup._domainUpperBound )
		setup._domainLowerBound = nval;
	else
		_domainLowerBound->set_value( static_cast<double>( setup._domainLowerBound ) );
	on_plot_data_changed();
}

void HWindowMain::on_domain_upper_bound_changed( void ) {
	double nval = _domainUpperBound->get_value();
	if ( nval > setup._domainLowerBound )
		setup._domainUpperBound = nval;
	else
		_domainUpperBound->set_value( static_cast<double>( setup._domainUpperBound ) );
	on_plot_data_changed();
}

void HWindowMain::on_range_lower_bound_changed( void ) {
	double nval = _rangeLowerBound->get_value();
	if ( nval < setup._rangeUpperBound )
		setup._rangeLowerBound = nval;
	else
		_rangeLowerBound->set_value( static_cast<double>( setup._rangeLowerBound ) );
	on_plot_data_changed();
}

void HWindowMain::on_range_upper_bound_changed( void ) {
	double nval = _rangeUpperBound->get_value();
	if ( nval > setup._rangeLowerBound )
		setup._rangeUpperBound = nval;
	else
		_rangeUpperBound->set_value( static_cast<double>( setup._rangeUpperBound ) );
	on_plot_data_changed();
}

namespace {

#pragma GCC diagnostic ignored "-Wold-style-cast"
void fwd_g_warning( char const* msg_ ) {
	g_warning( "%s", msg_ );
	return;
}
#pragma GCC diagnostic error "-Wold-style-cast"

}

void HWindowMain::get_value_for_cell( Gtk::CellRenderer* cell, Gtk::TreeModel::iterator const& iter, Gtk::TreeModelColumn<OPlotDesc> const& col ) {
	Gtk::CellRendererText* pTextRenderer = dynamic_cast<Gtk::CellRendererText*>(cell);
	if( ! ( pTextRenderer && ( !! iter ) ) )
		fwd_g_warning( "gtkmm: TextView: bad usage of value_getter." );
	else
		pTextRenderer->property_text() = iter->get_value( col )._formula.raw();
}

int gui_start( int argc_, char* argv_[] ) {
	M_PROLOG
	try {
		Gtk::Main gUI( argc_, argv_ );
		Glib::RefPtr<Gtk::Builder> resources( Gtk::Builder::create_from_file( setup._resourcePath.raw() ) );
		HWindowMain* windowMain = NULL;
		resources->get_widget_derived( "WINDOW_MAIN", windowMain );
		gUI.run( *windowMain );
	} catch ( Glib::Exception& e ) {
		std::cerr << "Glib::Exception: " << e.what() << std::endl;
		return ( -1 );
	} catch ( std::exception& e ) {
		std::cerr << "std::exception: " << e.what() << std::endl;
		return ( -1 );
	}
	return ( 0 );
	M_EPILOG
}

}

/* vim: set ft=cpp : */
