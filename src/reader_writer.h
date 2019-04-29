#ifndef __READER_WRITER_H__
#define __READER_WRITER_H__

/* Get nodes from a file
 *
 * @in filename: the name of the file
 * @out Data:    interlaced nodes data: X0,Y0,Value0,X1,Y1,Value1,...
 * @return:      the number of nodes
 */
unsigned read_nodes_txt(const char* filename, double** Data);


/* Write triangulation to file (either in txt or binary form)
 *
 * @in filename:  the name of the file
 * @in Data:      interlaced nodes data: X0,Y0,Value0,X1,Y1,Value1,...
 * @in dLength:   the number of nodes
 * @in Triangles: triangles node indices: T0_node0,T0_node1,T0_node2,T1_node0,T1_node1,T1_node2,
 * @in tLength:   the number of triangles
 */
void write_gmsh_txt(const char* filename, double* Data, unsigned dLength, unsigned* Triangles, unsigned tLength);
void write_gmsh_bin(const char* filename, double* Data, unsigned dLength, unsigned* Triangles, unsigned tLength);


#endif