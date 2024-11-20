import tkinter as tk
from tkinter import filedialog, messagebox
import json

class App:
    def __init__(self, root):
        self.root = root
        self.root.title("TM Parameter Editor")
        
        # SimulationParameters frame
        self.parameter_frame=tk.LabelFrame(self.root,text="Parameters")
        self.parameter_frame.pack(side=tk.TOP,padx=10,pady=10,fill='both',expand=True)
        self.simulation_frame = tk.LabelFrame(self.parameter_frame, text="Model Parameters")
        self.simulation_frame.pack(side=tk.LEFT,padx=10, pady=10, fill='both', expand=True)
        
        # DynamicParameters frame
        self.dynamic_frame = tk.LabelFrame(self.parameter_frame, text="Runtime Parameter")
        self.dynamic_frame.pack(side=tk.RIGHT,padx=10, pady=10, fill='both', expand=True)
        
        # Create labels and entries for SimulationParameters
        self.params = {
            "L0": tk.IntVar(),
            "Nr":tk.IntVar(),
            "mm": tk.DoubleVar(),
            "nn": tk.DoubleVar(),
            "R0": tk.DoubleVar(),
            "Eta0": tk.DoubleVar(),
            "dr": tk.DoubleVar(),
            "rs": tk.DoubleVar(),
            "q0": tk.DoubleVar(),
            "Ew": tk.DoubleVar(),
            "simT": tk.DoubleVar(),
            "savT": tk.DoubleVar(),
            "dt": tk.DoubleVar(),
        }
        for i, (key, var) in enumerate(self.params.items()):
            tk.Label(self.simulation_frame, text=key).grid(row=i, column=0, sticky='e', padx=5, pady=5)
            tk.Entry(self.simulation_frame, textvariable=var, width=10).grid(row=i, column=1, padx=5, pady=5)
        
        # Create labels and entries for DynamicParameters
        self.dynamic_params = {
            "count": tk.IntVar(),
            "Time": tk.DoubleVar(),
            "r0": tk.DoubleVar(),
            #"r": tk.StringVar(),  # List cannot be directly bound to an entry
            #"eta": tk.StringVar(),  # List cannot be directly bound to an entry
        }
        for i, (key, var) in enumerate(self.dynamic_params.items()):
            tk.Label(self.dynamic_frame, text=key).grid(row=i, column=0, sticky='e', padx=5, pady=5)
            if key in ['r', 'eta']:
                tk.Entry(self.dynamic_frame, textvariable=tk.StringVar(), width=10).grid(row=i, column=1, padx=5, pady=5)
                messagebox.showwarning("Warning", "Lists 'r' and 'eta' cannot be directly edited in this interface.")
            else:
                tk.Entry(self.dynamic_frame, textvariable=var, width=10).grid(row=i, column=1, padx=5, pady=5)
        
        # Buttons
        button_frame=tk.Frame(root)
        button_frame.pack(side=tk.BOTTOM,fill='x',padx=10,pady=10)
        tk.Button(button_frame, text="Load JSON Parameter", command=self.load_json).pack(side=tk.LEFT, padx=10, pady=10)
        tk.Button(button_frame, text="Save JSON Parameter", command=self.save_json).pack(side=tk.LEFT, padx=10, pady=10)
        tk.Button(button_frame, text="Write JSON TO Parameter.hpp", command=self.load_and_generate).pack(side=tk.RIGHT, padx=10, pady=10)

    # 读取JSON文件并生成头文件内容
    def generate_header_file(self,json_content):
        try:
            data = json.loads(json_content)

            simulation_params = data["SimulationParameters"]
            dynamic_params = data["DynamicParameters"]

            header_template = """
    //
    // Created by Administrator on 2024/7/24.
    //
    #ifndef TM_C_01_PARAMETER_HPP
    #define TM_C_01_PARAMETER_HPP
    //Fourier截止项
    // #define L0 1
    const int L0={L0};
    //网格节点数
    // #define Nr 1000
    const int Nr={Nr};

    const double mm = {mm};//极向模数
    const double nn = {nn};//环向模数
    const double R0 = {R0};//大半径
    const double Eta0 = {Eta0};//中心处电阻率
    const double dr = 1.0 / Nr;//径向步长

    const double rs = {rs};//初始有理面位置
    const double q0 = {q0};//初始中心安全因子
    const double Ew = 2 * Eta0 / q0;//边界电场

    const double simT = {simT};//模拟总时间
    const double savT = {savT};//数据保存时间
    const double dt = {dt};//模拟步长
    namespace Parameter {{
        int count;//储存文件编号
    }}
    double Time;//判断是否保存的时间
    double r0;//电流通道宽度
    double r[Nr + 1];//节点位置
    double eta[Nr + 1];//电阻率

    #endif //TM_C_01_PARAMETER_H
            """

            header_content = header_template.format(
                L0=simulation_params["L0"],
                Nr=simulation_params["Nr"],
                mm=simulation_params["mm"],
                nn=simulation_params["nn"],
                R0=simulation_params["R0"],
                Eta0=simulation_params["Eta0"],
                dr=simulation_params["dr"],
                rs=simulation_params["rs"],
                q0=simulation_params["q0"],
                Ew=simulation_params["Ew"],
                simT=simulation_params["simT"],
                savT=simulation_params["savT"],
                dt=simulation_params["dt"],
                count=dynamic_params["count"],
                Time=dynamic_params["Time"],
                r0=dynamic_params["r0"]
            )

            return header_content
        except Exception as e:
            messagebox.showerror("Error", str(e))
            return None

    # 保存头文件
    def save_header_file(self,header_content):
        output_hpp_path="Parameter.hpp"
        try:
            with open(output_hpp_path, 'w') as output_hpp:
                output_hpp.write(header_content)
            messagebox.showinfo("Success", f"Generated {output_hpp_path}")
        except Exception as e:
            messagebox.showerror("Error", str(e))

    # 选择JSON文件并生成头文件
    def load_and_generate(self):
        json_file_path = filedialog.askopenfilename(
            filetypes=[("JSON files", "*.json")]
        )
        if not json_file_path:
            return

        try:
            with open(json_file_path, 'r') as json_file:
                json_content = json_file.read()

            header_content = self.generate_header_file(json_content)
            if header_content:
                self.save_header_file(header_content)
        except Exception as e:
            messagebox.showerror("Error", str(e))

    def load_json(self):
        filename = filedialog.askopenfilename(filetypes=[("JSON files", "*.json")])
        if filename:
            with open(filename, 'r') as file:
                data = json.load(file)
                self.update_entries(data)

    def save_json(self):
        filename = filedialog.asksaveasfilename(defaultextension=".json", filetypes=[("JSON files", "*.json")])
        if filename:
            data = self.get_data()
            with open(filename, 'w') as file:
                json.dump(data, file, indent=4)
                messagebox.showinfo("Info", "JSON saved successfully")

    def update_entries(self, data):
        for key, var in self.params.items():
            var.set(data["SimulationParameters"].get(key, 0))
        for key, var in self.dynamic_params.items():
            var.set(data["DynamicParameters"].get(key, 0))

    def get_data(self):
        data = {
            "SimulationParameters": {key: var.get() for key, var in self.params.items()},
            "DynamicParameters": {key: var.get() for key, var in self.dynamic_params.items()}
        }
        return data

if __name__ == "__main__":
    root = tk.Tk()
    app = App(root)
    root.mainloop()