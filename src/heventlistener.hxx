/* Read booker/LICENSE.md file for copyright and licensing information. */

#ifndef FUNLAB_HEVENTLISTENER_HXX_INCLUDED
#define FUNLAB_HEVENTLISTENER_HXX_INCLUDED

namespace funlab {

template<typename event_t>
class HEventListener {
public:
	void on_event( event_t e )
		{ do_on_event( e ); }
	virtual ~HEventListener( void ) {}
protected:
	virtual void do_on_event( event_t ) = 0;
};

}

#endif /* not FUNLAB_HEVENTLISTENER_HXX_INCLUDED */

