a1 = 0
b1 = 0
c1 = 0
a2 = 0
b2 = 0
c2 = 0

encountered_e1 = []
encountered_e2 = []
encountered_e3 = []

for a1_i in range(2):
    a1 = (-1)**a1_i
    for a2_i in range(2):
        a2 = (-1)**a2_i
        for b1_i in range(2):
            b1 = (-1)**b1_i
            for b2_i in range(2):
                b2 = (-1)**b2_i
                for c1_i in range(2):
                    c1 = (-1)**c1_i
                    for c2_i in range(2):
                        c2 = (-1)**c2_i
                        e1 = a1*b1+a2*b2
                        e2 = a1*c1+a2*c2
                        e3 = b1*c1+b2*c2
                        if e1 not in encountered_e1:
                            encountered_e1.append(e1)
                        if e2 not in encountered_e2:
                            encountered_e2.append(e2)
                        if e3 not in encountered_e3:
                            encountered_e3.append(e3)
                        det = 2*(4-e3**2)-e1*(2*e1-e2*e3)+e2*(e1*e3-2*e2)
                        if det != 0:
                            print("There's a solution!")
