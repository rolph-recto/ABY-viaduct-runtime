#define alice 0

#define bob 1

#define Local__alice__at__alice 0

#define Replication__alice_bob__at__alice 1

#define Replication__alice_bob__at__bob 2

#define MPCWithAbort__alice_bob__at__alice 3

#define MPCWithAbort__alice_bob__at__bob 4

#define Local__bob__at__bob 5

void func_Local__alice__at__alice(ViaductProcessRuntime& runtime) {
    int tmp = runtime.receive(Replication__alice_bob__at__alice);
    int* a = new int[tmp];
    {
        while (true) {
            int tmp_4 = runtime.receive(Replication__alice_bob__at__alice);
            if (tmp_4) {
                int tmp_5 = runtime.receive(Replication__alice_bob__at__alice);
                int tmp_6 = runtime.input();
                a[tmp_5] = tmp_6;
                int tmp_9 = runtime.receive(Replication__alice_bob__at__alice);
                int tmp_10 = a[tmp_9];
                runtime.send(MPCWithAbort__alice_bob__at__alice, tmp_10);
            } else {
                break;
            }
        }
    }
    int tmp_18 = runtime.receive(Replication__alice_bob__at__alice);
    runtime.output(tmp_18);
    int tmp_20 = runtime.receive(Replication__alice_bob__at__alice);
    int tmp_21 = (tmp_20 < 100);
    runtime.output(tmp_21);
    delete[] a;
}

void func_Replication__alice_bob__at__alice(ViaductProcessRuntime& runtime) {
    int len = 5;
    int tmp = len;
    runtime.send(Local__alice__at__alice, tmp);
    int tmp_1 = len;
    {
        int i = 0;
        while (true) {
            int tmp_2 = i;
            int tmp_3 = len;
            int tmp_4 = (tmp_2 < tmp_3);
            runtime.send(Local__alice__at__alice, tmp_4);
            runtime.send(MPCWithAbort__alice_bob__at__alice, tmp_4);
            if (tmp_4) {
                int tmp_5 = i;
                runtime.send(Local__alice__at__alice, tmp_5);
                int tmp_7 = i;
                int tmp_9 = i;
                runtime.send(Local__alice__at__alice, tmp_9);
                int tmp_12 = i;
                i = (i + 1);
            } else {
                break;
            }
        }
    }
    int tmp_16 = runtime.receive(MPCWithAbort__alice_bob__at__alice);
    int tmp_17 = tmp_16;
    int prodsum_public = tmp_17;
    int tmp_18 = prodsum_public;
    runtime.send(Local__alice__at__alice, tmp_18);
    int tmp_19 = prodsum_public;
    int tmp_20 = prodsum_public;
    runtime.send(Local__alice__at__alice, tmp_20);
}

void func_Replication__alice_bob__at__bob(ViaductProcessRuntime& runtime) {
    int len = 5;
    int tmp = len;
    int tmp_1 = len;
    runtime.send(Local__bob__at__bob, tmp_1);
    {
        int i = 0;
        while (true) {
            int tmp_2 = i;
            int tmp_3 = len;
            int tmp_4 = (tmp_2 < tmp_3);
            runtime.send(MPCWithAbort__alice_bob__at__bob, tmp_4);
            runtime.send(Local__bob__at__bob, tmp_4);
            if (tmp_4) {
                int tmp_5 = i;
                int tmp_7 = i;
                runtime.send(Local__bob__at__bob, tmp_7);
                int tmp_9 = i;
                int tmp_12 = i;
                runtime.send(Local__bob__at__bob, tmp_12);
                i = (i + 1);
            } else {
                break;
            }
        }
    }
    int tmp_16 = runtime.receive(MPCWithAbort__alice_bob__at__bob);
    int tmp_17 = tmp_16;
    int prodsum_public = tmp_17;
    int tmp_18 = prodsum_public;
    int tmp_19 = prodsum_public;
    runtime.send(Local__bob__at__bob, tmp_19);
    int tmp_20 = prodsum_public;
}

void func_MPCWithAbort__alice_bob__at__alice(ViaductProcessRuntime& runtime, CachedCircuit& circuit) {
    CircuitGate* prodsum = circuit.PutCONSTGate(0);
    {
        while (true) {
            int tmp_4 = runtime.receive(Replication__alice_bob__at__alice);
            CircuitGate* tmp_4__share = circuit.PutINGate(tmp_4);
            if (tmp_4) {
                int tmp_10 = runtime.receive(Local__alice__at__alice);
                CircuitGate* tmp_10__share = circuit.PutINGate(tmp_10);
                CircuitGate* tmp_11 = tmp_10__share;
                CircuitGate* tmp_13__share = circuit.PutDummyINGate();
                CircuitGate* tmp_14 = tmp_13__share;
                CircuitGate* tmp_15 = circuit.PutMULGate(tmp_11, tmp_14);
                prodsum = circuit.PutADDGate(prodsum, tmp_15);
            } else {
                break;
            }
        }
    }
    CircuitGate* tmp_16 = prodsum;
    circuit.Reset();
    share* tmp_16__share = circuit.BuildCircuit(tmp_16);
    circuit.ExecCircuit();
    int tmp_16__clear = (*tmp_16__share).get_clear_value<int>();
    runtime.send(Replication__alice_bob__at__alice, tmp_16__clear);
}

void func_MPCWithAbort__alice_bob__at__bob(ViaductProcessRuntime& runtime, CachedCircuit& circuit) {
    CircuitGate* prodsum = circuit.PutCONSTGate(0);
    {
        while (true) {
            int tmp_4 = runtime.receive(Replication__alice_bob__at__bob);
            CircuitGate* tmp_4__share = circuit.PutINGate(tmp_4);
            if (tmp_4) {
                CircuitGate* tmp_10__share = circuit.PutDummyINGate();
                CircuitGate* tmp_11 = tmp_10__share;
                int tmp_13 = runtime.receive(Local__bob__at__bob);
                CircuitGate* tmp_13__share = circuit.PutINGate(tmp_13);
                CircuitGate* tmp_14 = tmp_13__share;
                CircuitGate* tmp_15 = circuit.PutMULGate(tmp_11, tmp_14);
                prodsum = circuit.PutADDGate(prodsum, tmp_15);
            } else {
                break;
            }
        }
    }
    CircuitGate* tmp_16 = prodsum;
    circuit.Reset();
    share* tmp_16__share = circuit.BuildCircuit(tmp_16);
    circuit.ExecCircuit();
    int tmp_16__clear = (*tmp_16__share).get_clear_value<int>();
    runtime.send(Replication__alice_bob__at__bob, tmp_16__clear);
}

void func_Local__bob__at__bob(ViaductProcessRuntime& runtime) {
    int tmp_1 = runtime.receive(Replication__alice_bob__at__bob);
    int* b = new int[tmp_1];
    {
        while (true) {
            int tmp_4 = runtime.receive(Replication__alice_bob__at__bob);
            if (tmp_4) {
                int tmp_7 = runtime.receive(Replication__alice_bob__at__bob);
                int tmp_8 = runtime.input();
                b[tmp_7] = tmp_8;
                int tmp_12 = runtime.receive(Replication__alice_bob__at__bob);
                int tmp_13 = b[tmp_12];
                runtime.send(MPCWithAbort__alice_bob__at__bob, tmp_13);
            } else {
                break;
            }
        }
    }
    int tmp_19 = runtime.receive(Replication__alice_bob__at__bob);
    runtime.output(tmp_19);
    delete[] b;
}

void start(ViaductRuntime& runtime, ABYInfo abyInfo) {
    LocalProcess proc_Local__alice__at__alice(func_Local__alice__at__alice);
    ReplicatedProcess proc_Replication__alice_bob__at__alice(func_Replication__alice_bob__at__alice);
    ReplicatedProcess proc_Replication__alice_bob__at__bob(func_Replication__alice_bob__at__bob);
    MPCProcess proc_MPCWithAbort__alice_bob__at__alice(abyInfo, func_MPCWithAbort__alice_bob__at__alice);
    MPCProcess proc_MPCWithAbort__alice_bob__at__bob(abyInfo, func_MPCWithAbort__alice_bob__at__bob);
    LocalProcess proc_Local__bob__at__bob(func_Local__bob__at__bob);
    runtime.registerProcess(Local__alice__at__alice, 0, proc_Local__alice__at__alice);
    runtime.registerProcess(Replication__alice_bob__at__alice, 0, proc_Replication__alice_bob__at__alice);
    runtime.registerProcess(Replication__alice_bob__at__bob, 1, proc_Replication__alice_bob__at__bob);
    runtime.registerProcess(MPCWithAbort__alice_bob__at__alice, 0, proc_MPCWithAbort__alice_bob__at__alice);
    runtime.registerProcess(MPCWithAbort__alice_bob__at__bob, 1, proc_MPCWithAbort__alice_bob__at__bob);
    runtime.registerProcess(Local__bob__at__bob, 1, proc_Local__bob__at__bob);
    runtime.run();
}