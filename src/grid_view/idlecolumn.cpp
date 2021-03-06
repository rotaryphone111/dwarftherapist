/*
Dwarf Therapist
Copyright (c) 2009 Trey Stout (chmod)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "idlecolumn.h"
#include "columntypes.h"
#include "viewcolumnset.h"
#include "dwarfmodel.h"
#include "dwarf.h"
#include "dwarftherapist.h"
#include "defines.h"
#include "dwarfjob.h"
#include "gamedatareader.h"

IdleColumn::IdleColumn(const QString &title, ViewColumnSet *set, QObject *parent) 
    : ViewColumn(title, CT_IDLE, set, parent)
{}

IdleColumn::IdleColumn(const IdleColumn &to_copy)
    : ViewColumn(to_copy)
{}

QStandardItem *IdleColumn::build_cell(Dwarf *d) {
    QStandardItem *item = init_cell(d);
    short job_id = d->current_job_id();
	QString pixmap_name(":img/help.png");
    QString tooltip;
    if(d->is_dead()){
        pixmap_name = ":/status/img/Skull-Icon.png";
        tooltip = QString("<h3>%1</h3>%2 (%3)<h4>%4</h4>")
        .arg(m_title)
        .arg("Dead")
        .arg(-2)
        .arg(d->nice_name());
        m_cells[d]->setData(-1, DwarfModel::DR_SORT_VALUE);
    }
    else{
	    if (job_id == -1) {
		    pixmap_name = ":status/img/bullet_red.png"; // idle
	    } else {
		    DwarfJob *job = GameDataReader::ptr()->get_job(job_id);
		    if (job) {
			    switch (job->type) {
			    case DwarfJob::DJT_IDLE:	pixmap_name = ":status/img/bullet_red.png"; 		break;
			    case DwarfJob::DJT_DIG:		pixmap_name = ":status/img/pickaxe.png";			break;
                case DwarfJob::DJT_CUT:     pixmap_name = ":status/img/axe.png";                break;
			    case DwarfJob::DJT_REST:	pixmap_name = ":status/img/status_sleep.png";		break;
			    case DwarfJob::DJT_DRINK:	pixmap_name = ":status/img/status_drink.png";		break;
			    case DwarfJob::DJT_FOOD:	pixmap_name = ":status/img/cheese.png"; 			break;
			    case DwarfJob::DJT_BUILD:	pixmap_name = ":status/img/gear.png";       		break;
			    case DwarfJob::DJT_HAUL:	pixmap_name = ":status/img/status_haul.png";	   	break;
                case DwarfJob::DJT_FIGHT:	pixmap_name = ":status/img/status_fight2.png";	    break;
                case DwarfJob::DJT_MOOD:	pixmap_name = ":img/exclamation.png";       	    break;
                case DwarfJob::DJT_FORGE:	pixmap_name = ":status/img/status_forge.png"; 	    break;
    			
			    default:
			    case DwarfJob::DJT_DEFAULT:	pixmap_name = ":status/img/control_play_blue.png";	break;
			    }
		    }
	    }
        item->setData(d->current_job_id(), DwarfModel::DR_SORT_VALUE);
        tooltip = QString("<h3>%1</h3>%2 (%3)<h4>%4</h4>")
            .arg(m_title)
            .arg(d->current_job())
            .arg(d->current_job_id())
            .arg(d->nice_name());
    }
    item->setData(CT_IDLE, DwarfModel::DR_COL_TYPE);
    item->setData(QIcon(pixmap_name), Qt::DecorationRole);
    item->setToolTip(tooltip);
    return item;
}

QStandardItem *IdleColumn::build_aggregate(const QString &group_name, const QVector<Dwarf*> &dwarves) {
    Q_UNUSED(group_name);
    Q_UNUSED(dwarves);
    QStandardItem *item = new QStandardItem;
    item->setData(m_bg_color, DwarfModel::DR_DEFAULT_BG_COLOR);
    return item;
}

void IdleColumn::redraw_cells(){
	foreach(Dwarf *d, m_cells.uniqueKeys()) {
		if (d && d->can_read && (d->is_dead() || d->get_dirty().D_JOB) && m_cells[d] && m_cells[d]->model()){
            QString pixmap_name(":img/help.png");
            QString tooltip;
            if(d->is_dead()){
                pixmap_name = ":/status/img/Skull-Icon.png";
                tooltip = QString("<h3>%1</h3>%2 (%3)<h4>%4</h4>")
                .arg(m_title)
                .arg("Dead")
                .arg(-2)
                .arg(d->nice_name());
                m_cells[d]->setData(-1, DwarfModel::DR_SORT_VALUE);
            }
            else{
                short job_id = d->current_job_id();
                if (job_id == -1) {
	                pixmap_name = ":status/img/bullet_red.png"; // idle
                } else {
	                DwarfJob *job = GameDataReader::ptr()->get_job(job_id);
                if (job) {
	                switch (job->type) {
	                case DwarfJob::DJT_IDLE:	pixmap_name = ":status/img/bullet_red.png"; 		break;
	                case DwarfJob::DJT_DIG:		pixmap_name = ":status/img/pickaxe.png";			break;
	                case DwarfJob::DJT_CUT:     pixmap_name = ":status/img/axe.png";                break;
	                case DwarfJob::DJT_REST:	pixmap_name = ":status/img/status_sleep.png";		break;
	                case DwarfJob::DJT_DRINK:	pixmap_name = ":status/img/status_drink.png";		break;
	                case DwarfJob::DJT_FOOD:	pixmap_name = ":status/img/cheese.png"; 			break;
	                case DwarfJob::DJT_BUILD:	pixmap_name = ":status/img/gear.png";       		break;
	                case DwarfJob::DJT_HAUL:	pixmap_name = ":status/img/status_haul.png";	   	break;
	                case DwarfJob::DJT_FIGHT:	pixmap_name = ":status/img/status_fight2.png";	    break;
	                case DwarfJob::DJT_MOOD:	pixmap_name = ":img/exclamation.png";       	    break;
	                case DwarfJob::DJT_FORGE:	pixmap_name = ":status/img/status_forge.png"; 	    break;
    				
	                default:
	                case DwarfJob::DJT_DEFAULT:	pixmap_name = ":status/img/control_play_blue.png";	break;
	                }
                    }
                         
	            m_cells[d]->setData(d->current_job_id(), DwarfModel::DR_SORT_VALUE);
	            tooltip = QString("<h3>%1</h3>%2 (%3)<h4>%4</h4>")
		            .arg(m_title)
		            .arg(d->current_job())
		            .arg(d->current_job_id())
		            .arg(d->nice_name());
                }
            }
        m_cells[d]->setData(QIcon(pixmap_name), Qt::DecorationRole);
        m_cells[d]->setToolTip(tooltip);         
        }
    }
}