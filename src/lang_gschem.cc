/* This file is the plugin for the gEDa/gschem plugin
 * The documentation of how this should work is at bit.ly(slash)gnucapwiki
 * 
 * This file is part of "Gnucap", the Gnu Circuit Analysis Package.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 */
#include "c_comand.h"
#include "d_dot.h"
#include "d_coment.h"
#include "d_subckt.h"
#include "e_model.h"
#include "u_lang.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class LANG_GSCHEM : public LANGUAGE {

  public:
//functions to be declared
  private:


}lang_gschem;

DISPATCHER<LANGUAGE>::INSTALL d(&language_dispatcher, lang_gschem.name(),&lang_gschem)
/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
//functions to be defined



class CMD_GSCHEM : public CMD {
public:
    void do_it(CS&, CARD_LIST* Scope)
    {
    command("options lang=gschem", Scope);
    }
} p8;
DISPATCHER<CMD>::INSTALL d8(&command_dispatcher, "gschem", &p8);
/*----------------------------------------------------------------------*/
}
/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

