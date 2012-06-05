node=[
['47300','44500','node1_1'] ,
['46400','44500','node1_2'] ,
['48600','44500','node2_1'] ,
['47700','44500','node2_2'] ,
['48700','47800','node3_1'] ,
['48700','46900','node3_2'] ,
['47500','44200','node4_1'] ,
['47500','43300','node4_2'] ,
['46300','46400','node5_1'] ,
['46300','45400','node5_2'] ,
['45700','45900','node5_3'] ,
['48700','46400','node6_1'] ,
['48700','45400','node6_2'] ,
['49300','45900','node6_3'] ,
['47500','47800','node7_1'] ,
['47500','43300','node8_1'] ,
['49300','45900','node9_1'] ,
['45700','45900','node10_1'] ,
['48700','46600','node11_1'] ,
['46300', '47800', 'node0'] ,
['46300', '44500', 'node1'],
['48700', '44500', 'node2'],
['47500', '44500', 'node3']
]
net=[
['net1','node0','node5_1']    ,
['net2','node5_2','node1'] ,
['net3','node1','node1_2'] ,
['net4','node1_1','node2_2'], 
['net5','node2_1','node2'] ,
['net6','node2','node6_2'] ,
['net7','node4_1','node3'] ,
['net8','node3_2','node6_1'], 
['net9','node0','node3_1'] ,
['net10','node4_2','node8_1'], 
['net11','node5_3','node10_1'], 
['net12','node6_3','node9_1'] 
]


def nodeinnet(d,n):
	for e in node:
		if e[2]==n[1]:
			n1x=int(e[0])
			n1y=int(e[1])
		if e[2]==n[2]:
			n2x=int(e[0])
			n2y=int(e[1])
	dx=int(d[0])
	dy=int(d[1])
	if n2y==n1y:
		if (n2x<=dx<=n1x or n1x<=dx<=n2x) and dy==n1y:
			return True
		else:
			return False
	elif n2x==n1x:
		if (n2y<=dy<=n1y or n1y<=dy<=n2y) and dx==n1x:
			return True
		else:
			return False
	else:
		return False
	
	
for n in net:
	for f in node:
		if not (f[2]==n[1] or f[2]==n[2]):
			if nodeinnet(f,n):
				print f[2]+' is in '+ n[0]+' :'+n[1]
	
	'''
	if not n2x==n1x:
		slopenet=float((n2y-n1y)/(n2x-n1x))
		slopenode=float((n2y-dy)/(n2x-dx))
	elif not n2y==dy:
		slopenet=float((n2x-n1x)/(n2y-n1y))
		slopenode=float((n2x-dx)/(n2y-dy))
	if slopenode==slopenet:
		return True
	else:
		return False
	'''	
'''
index=0;
z=raw_input()
while not z=='s':
	fi=open('node.txt','w')
	nx=raw_input("Give x :")
	ny=raw_input("Give y :")
	fl=0
	for e in node:
		fi.write(str(e)+'\n')
		if e[0]==nx and e[1]==ny :
			print e[2]
			fl=1
			
	if not fl:
		print "Not found"
		nodename='node'+str(index)
		node.append([nx,ny','nodename])
		print 'node'+str(index)
		index=index+1;
	z=raw_input()

for e in node:
	for f in node:
		if not e[2]==f[2]:
			if e[0]==f[0] and e[1]==f[1]:
				print e[2]+' '+f[2]
'''


