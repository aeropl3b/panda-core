#pragma once

namespace panda
{
namespace core
{

template < class VID, class VHash >
class graph
{
public:

   using index_t = std::size_t;
   using vertex_id = typename std::remove_cv<typename std::remove_reference<VID>::type>::type;

   class vertex
   {
      vertex_id_ _id;
      double _weight = 1.;
   public:
      vertext_id_t id() const { return _id; }
      double weight() const { return _weight; }
   };

   class edge
   {
      index_t _id;
      vertex _v0,_v1;
      double _weight = 1.;
   public:
      index_t id() const { return _id; }
      double weight() const { return _weight; }
   };

   graph() = default;

   /** Add a vertex to the graph
    * @return created vertex
    */
   vertex& add_vertex();
   
   /** Add a weighted vertex to the graph
    * @return created vertex
    */
   vertex& add_vertex(double weight);

   /** Adds an edge connecting v0 -> v1, or if bi_directional then v0 <=> v1
    * @param v0 vertex
    * @param v1 vertex
    * @param bi_direction flag to denote if the edge is directional (true) or non-directional (false)
    * @return created edge
    */
   edge& add_edge(const vertex& v0, const vertex& v1, bool directional = false);

   /** Adds an edge connecting v0 <=> v1, or if directional then v0 -> v1
    * @param v0 vertex
    * @param v1 vertex
    * @param weight weight of the edge
    * @param bi_direction flag to denote if the edge is directional (true) or non-directional (false)
    * @return created edge
    */
   edge& add_edge(const vertex& v0, const vertex& v1, double weight, bool directional = false);

   std::vector<vertex> shortest_path_dijkstra(const vertex& v0, const vertex& v1);

   /**
    */
   void partition(const network::domain* d);

private:

   std::vector<index_t> _vertex_connectivity;
   std::vector<index_t> _vertex_connectivity_start;
   std::unordered_map<vertex_id_t,index_t,VHash> _lookup_vertex;

   dynamic_soa<std::unordered_map> _vertex_data;
   dynamic_soa<std::unordered_map> _edge_data;
};

}
}
