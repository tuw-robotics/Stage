///////////////////////////////////////////////////////////////////////////
//
// File: laserbeacon.cc
// Author: Andrew Howard
// Date: 4 Dec 2000
// Desc: Simulates the Player CLaserDevice (the SICK laser)
//
// CVS info:
//  $Source: /home/tcollett/stagecvs/playerstage-cvs/code/stage/src/laserbeacon.cc,v $
//  $Author: ahoward $
//  $Revision: 1.2 $
//
// Usage:
//  This object acts a both a simple laser reflector and a more complex
//  laser beacon with an id.
//
// Theory of operation:
//  (empty)
//
// Known bugs:
//  (empty)
//
// Possible enhancements:
//  (empty)
//
///////////////////////////////////////////////////////////////////////////

#define ENABLE_RTK_TRACE 1

#include "world.hh"
#include "laserbeacon.hh"


///////////////////////////////////////////////////////////////////////////
// Default constructor
//
CLaserBeacon::CLaserBeacon(CWorld *world, CEntity *parent)
        : CEntity(world, parent)
{
    m_beacon_id = 0;
    m_index = -1;
    
    // Set the initial map pose
    //
    m_map_px = m_map_py = m_map_pth = 0;

    exp.objectType = laserbeacon_o;
    exp.width = 0.02;
    exp.height = 0.25;
    strcpy( exp.label, "Beacon" );
}


///////////////////////////////////////////////////////////////////////////
// Load the object from an argument list
//
bool CLaserBeacon::Load(int argc, char **argv)
{
    if (!CEntity::Load(argc, argv))
        return false;

    for (int i = 0; i < argc;)
    {
        // Extract id
        //
        if (strcmp(argv[i], "id") == 0 && i + 1 < argc)
        {
            strcpy(m_id, argv[i + 1]);
            m_beacon_id = atoi(argv[i + 1]);
            i += 2;
        }
        else
            i++;
    }

#ifdef INCLUDE_RTK
    m_mouse_radius = (m_parent_object == NULL ? 0.2 : 0.0);
    m_draggable = (m_parent_object == NULL);
#endif
        
    return true;
}


///////////////////////////////////////////////////////////////////////////
// Save the object
//
bool CLaserBeacon::Save(int &argc, char **argv)
{
    if (!CEntity::Save(argc, argv))
        return false;

    // Save id
    //
    char id[32];
    snprintf(id, sizeof(id), "%d", (int) m_beacon_id);
    argv[argc++] = strdup("id");
    argv[argc++] = strdup(id);
    
    return true;
}


///////////////////////////////////////////////////////////////////////////
// Startup routine
//
bool CLaserBeacon::Startup()
{
    assert(m_world != NULL);
    m_index = m_world->AddLaserBeacon(m_beacon_id);
    return CEntity::Startup();
}


///////////////////////////////////////////////////////////////////////////
// Update the laser data
//
void CLaserBeacon::Update()
{
    //RTK_TRACE0("updating laser beacon");
    ASSERT(m_world != NULL);

    // Undraw our old representation
    //
    m_world->SetCell(m_map_px, m_map_py, layer_laser, 0);

    // Update our global pose
    //
    GetGlobalPose(m_map_px, m_map_py, m_map_pth);
    
    // Draw our new representation
    //
    m_world->SetCell(m_map_px, m_map_py, layer_laser, 2);
    m_world->SetLaserBeacon(m_index, m_map_px, m_map_py, m_map_pth);
}


#ifdef INCLUDE_RTK

///////////////////////////////////////////////////////////////////////////
// Process GUI update messages
//
void CLaserBeacon::OnUiUpdate(RtkUiDrawData *data)
{
    CEntity::OnUiUpdate(data);

    data->begin_section("global", "");
    
    if (data->draw_layer("laser_beacon", true))
    {
        double r = 0.10;
        double ox, oy, oth;
        GetGlobalPose(ox, oy, oth);
        double dx = 2 * r * cos(oth);
        double dy = 2 * r * sin(oth);
        data->set_color(RTK_RGB(255, 0, 0));
        data->ellipse(ox - r, oy - r, ox + r, oy + r);
        data->line(ox - dx, oy - dy, ox + dx, oy + dy);
    }

    data->end_section();
}


///////////////////////////////////////////////////////////////////////////
// Process GUI mouse messages
//
void CLaserBeacon::OnUiMouse(RtkUiMouseData *data)
{
    CEntity::OnUiMouse(data);
}

#endif




