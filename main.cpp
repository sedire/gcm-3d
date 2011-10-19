#include <iostream>
#include "gcm-3d.h"

using std::cout;

int main()
{
	srand( unsigned( time(0)) ) ;

	Logger* log = new Logger();
	GCM_Tetr_Plastic_Interpolation_1stOrder_Advanced* nm = new GCM_Tetr_Plastic_Interpolation_1stOrder_Advanced();
	//GCM_Tetr_Plastic_Interpolation_1stOrder* nm = new GCM_Tetr_Plastic_Interpolation_1stOrder();
	VoidRheologyCalculator* rc = new VoidRheologyCalculator();
	TetrMesh_1stOrder* mesh = new TetrMesh_1stOrder();
	Stresser* stresser = new GCMStresser();
	stresser->loadTask("task.xml");

	log->write(string("Mesh type: ") + *(mesh->get_mesh_type()));

	mesh->attach(log);
	mesh->attach(rc);
	mesh->attach(nm);
	mesh->attach(stresser);

	// TODO - check cube-self-gen
	mesh->load_msh_file((char*)"./tube-with-crack.msh");
//	mesh->load_msh_file((char*)"./data/models/cube-self-gen.msh");
//	mesh->load_msh_file((char*)"/home/anganar/Dropbox/Public/models/tube-with-crack.msh");
//	mesh->load_gmv_file((char*)"mesh-optimized.gmv");
//	mesh->load_node_ele_files((char*)"data/models/heart.node",(char*)"data/models/heart.ele");
//	mesh->load_node_ele_files((char*)"data/models/cube.1.node",(char*)"data/models/cube.1.ele");

	mesh->pre_process_mesh(1);

	TaskPreparator* tp = new TaskPreparator();
	tp->set_fixed_elastic_rheology(&(mesh->nodes), 70000, 10000, 1, 1000000);
	SnapshotWriter* sw = new SnapshotWriter();
	sw->attach(log);
//	sw->set_basement(0.1,0.1,0.1,2.44,20); // It is required for cubic dump only
	mesh->log_quality_stats();

	if (sw->tmp_dump_line(mesh,0) < 0)
	{
		cout << "Can not dump!\n";
		return -1;
	}
	for(int i = 1; i < 100; i++)
	{
		cout << "Started step " << i << ". Time = " << mesh->get_current_time() << "." << endl;
		for(int j = 0; j < 10; j++)
			if (mesh->do_next_step() < 0)
				return -1;
		if (sw->tmp_dump_line(mesh,i) < 0)
			return -1;
		cout << "Finished step " << i << ". Time = " << mesh->get_current_time() << "." << endl;
	}
	return 0;
}
