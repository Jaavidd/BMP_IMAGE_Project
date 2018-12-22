from Dijkstra.Vertex   import Vertex
from Dijkstra.Edge import Edge
from  Dijkstra.Algorithm import Algorithm



node1=Vertex("A")
node2=Vertex("B")
node3=Vertex("C")
node4=Vertex("D")
node5=Vertex("E")

edge1=Edge(7,node1,node2)
edge2=Edge(3,node2,node3)
edge3=Edge(8,node1,node3)
edge4=Edge(1,node3,node4)
edge5=Edge(1,node4,node5)
edge6=Edge(11,node1,node5)


node1.adjacenciesList.append(edge1)
node1.adjacenciesList.append(edge3)
node1.adjacenciesList.append(edge6)

node2.adjacenciesList.append(edge2)
node3.adjacenciesList.append(edge4)
node4.adjacenciesList.append(edge5)


vertexList={node1,node2,node3,node4,node5}

algorithm=Algorithm()
algorithm.calculatePath(vertexList,node1)
algorithm.getPath(node5)