import json
import os
import pandas as pd
import matplotlib.pyplot as plt


class PlotTool:
    def __init__(self, config_path):
        # 加载JSON配置文件
        with open(config_path, 'r', encoding='utf-8') as f:
            self.config = json.load(f)

    def load_data(self, path, prefix=None):
        """
        从单个文本文件或文件夹中加载数据。
        如果是文件夹，按文件名从 '00000.txt' 开始读取，每次递增1。
        """
        if os.path.isfile(path):  # 如果是单个文件
            data_frames = [pd.read_csv(path, sep=',', encoding='utf-8')]
        elif os.path.isdir(path):  # 如果是文件夹
            if prefix is None:
                raise ValueError("读取文件夹时必须指定文件名前缀。")
            # 过滤符合命名规则的文件名并按递增顺序排序
            files = sorted([
                os.path.join(path, file)
                for file in os.listdir(path)
                if file.startswith(prefix) and file.endswith('.txt')
            ], key=lambda x: int(os.path.splitext(os.path.basename(x))[0][len(prefix):]))
            if not files:
                raise ValueError(f"文件夹中未找到以 '{prefix}' 为前缀的文件。")
            data_frames = [pd.read_csv(file, sep=',', encoding='utf-8') for file in files]
        else:
            raise ValueError("提供的路径既不是文件也不是文件夹，请检查路径。")
        # 合并所有数据
        return pd.concat(data_frames, ignore_index=True)

    def plot_line_chart(self, data, x_col, y_cols):
        """
        绘制折线图（包括多条折线）
        """
        config = self.config['line_chart']
        plt.figure(figsize=(10, 6))
        for idx, y_col in enumerate(y_cols):
            style = config['line_styles'][idx]
            plt.plot(data[x_col], data[y_col],
                     color=style['color'],
                     linestyle=style['linestyle'],
                     label=style['label'])
        plt.title(config['title'])
        plt.xlabel(config['xlabel'])
        plt.ylabel(config['ylabel'])
        plt.legend()
        plt.grid(True)
        plt.show()

    def plot_polar_chart(self, data, theta_col, r_col):
        """
        绘制极坐标图
        """
        config = self.config['polar_chart']
        plt.figure(figsize=(8, 8))
        ax = plt.subplot(111, polar=True)
        style = config['line_styles'][0]
        ax.plot(data[theta_col], data[r_col],
                color=style['color'],
                linestyle=style['linestyle'],
                label=style['label'])
        ax.set_title(config['title'], va='bottom')
        ax.legend()
        plt.show()

    def save_plot(self, filename):
        """
        保存图像到文件
        """
        plt.savefig(filename, dpi=300)
        print(f"图像已保存到 {filename}")


# 示例使用
if __name__ == "__main__":
    # 初始化工具类
    tool = PlotTool("config.json")

    # 从单个文本文件加载数据
    data = tool.load_data("data.txt")

    # 绘制折线图
    tool.plot_line_chart(data, x_col="x", y_cols=["y1", "y2"])

    # 从文件夹加载数据并合并（文件名前缀为 'data_'）
    folder_data = tool.load_data("data_folder", prefix="data_")

    # 绘制极坐标图
    tool.plot_polar_chart(folder_data, theta_col="theta", r_col="radius")
